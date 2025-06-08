#include <core/file.hpp>

#include <opengl/buffer.hpp>
#include <opengl/commands.hpp>
#include <opengl/functions.hpp>
#include <opengl/pipeline.hpp>
#include <opengl/shader.hpp>
#include <opengl/vertex_array.hpp>

#include <opengl/constants/buffer.hpp>
#include <opengl/constants/commands.hpp>
#include <opengl/constants/pipeline.hpp>
#include <opengl/constants/shader.hpp>

#include <shaders/converter.hpp>

#include "piece_type.hpp"

int32_t tiles[3][3] { };
auto x_turn = true;
auto is_end = false;

btCollisionWorld* world;

glm::mat4 view;
glm::mat4 proj;

auto check_row(const int32_t row, const int32_t type) -> bool
{
    return tiles[row][0] == type &&
           tiles[row][1] == type &&
           tiles[row][2] == type;
}

auto check_col(const int32_t col, const int32_t type) -> bool
{
    return tiles[0][col] == type &&
           tiles[1][col] == type &&
           tiles[2][col] == type;
}

auto check_diagonals(const int32_t type) -> bool
{
    return tiles[0][0] == type &&
           tiles[1][1] == type &&
           tiles[2][2] == type ||
           tiles[0][2] == type &&
           tiles[1][1] == type &&
           tiles[2][0] == type;
}

auto check_win(const int32_t type) ->  void
{
    for (auto row = 0; row < 3; row++)
    {
        is_end = check_row(row, type);

        if (is_end)
        {
            return;
        }
    }

    for (auto col = 0; col < 3; col++)
    {
        is_end = check_col(col, type);

        if (is_end)
        {
            return;
        }
    }

    is_end = check_diagonals(type);
}

auto main() -> int32_t
{
    shaders::Converter::convert("../../resources/shaders", ".");

    constexpr auto window_width  = 1000;
    constexpr auto window_height = 1000;

       static auto window_closed = false;

       static auto cursor_x = 0.0f;
       static auto cursor_y = 0.0f;

    if (glfwInit() != GLFW_TRUE)
    {
        return -1;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    const auto window = glfwCreateWindow(window_width, window_height, "Tic-Tac-Toe", nullptr);

    glfwSetKeyCallback(window, [](const int key, int, const int action, int) -> void
    {
        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        {
            for (auto row = 0; row < 3; row++)
            {
                for (auto col = 0; col < 3; col++)
                {
                    tiles[row][col] = piece_empty;
                }
            }

            x_turn = true;
            is_end = false;
        }

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            window_closed = true;
        }
    });

    glfwSetCursorPosCallback(window, [](const double x, const double y) -> void
    {
        cursor_x = x;
        cursor_y = y;
    });

    glfwSetMouseButtonCallback(window, [](const int button, const int action, const int) -> void
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !is_end)
        {
            constexpr glm::vec4 viewport { 0.0f, 0.0f, window_width, window_height };

            auto start = glm::unProject(glm::vec3(cursor_x, window_height - cursor_y, -1.0f), view, proj, viewport);
            auto end   = glm::unProject(glm::vec3(cursor_x, window_height - cursor_y,  1.0f), view, proj, viewport);
                 end   = start + glm::normalize(end - start) * 1000.0f;

            const btVector3 from(start.x, start.y, start.z);
            const btVector3   to(  end.x,   end.y,   end.z);

            btCollisionWorld::ClosestRayResultCallback result(from, to);
                                               world->rayTest(from, to, result);
            if (result.hasHit())
            {
                const auto row = result.m_collisionObject->getUserIndex();
                const auto col = result.m_collisionObject->getUserIndex2();

                if (tiles[row][col] == piece_empty)
                {
                    if (x_turn)
                    {
                        tiles[row][col] = piece_x;
                        check_win(piece_x);
                    }
                    else
                    {
                        tiles[row][col] = piece_o;
                        check_win(piece_o);
                    }

                    x_turn = !x_turn;
                }
            }
        }
    });

    glfwSetWindowCloseCallback(window, []
    {
        window_closed = true;
    });

    glfwMakeContextCurrent(window);

    opengl::Functions::init();

    opengl::ShaderStage base_shader_vert;
    base_shader_vert.type(opengl::constants::vertex_shader);
    base_shader_vert.create();
    base_shader_vert.source(core::File::read("default_base_shader.vert", std::ios::binary));

    opengl::ShaderStage base_shader_frag;
    base_shader_frag.type(opengl::constants::fragment_shader);
    base_shader_frag.create();
    base_shader_frag.source(core::File::read("default_base_shader.frag", std::ios::binary));

    opengl::Shader base_shader;
    base_shader.create();
    base_shader.attach(base_shader_vert);
    base_shader.attach(base_shader_frag);
    base_shader.link();

    constexpr core::vertex_array::attribute position_attribute { 0, 3, opengl::constants::float_type, 0 };

    Assimp::Importer pieces_importer;

    std::vector<glm::vec3> o_vertices;
    std::vector<uint32_t>  o_elements;

    const auto pieces = pieces_importer.ReadFile("grid_pieces.obj", 0);
    const auto o_mesh = pieces->mMeshes[1];
    const auto x_mesh = pieces->mMeshes[0];

    for (auto i = 0; i < o_mesh->mNumVertices; i++)
    {
        const auto& vertex = o_mesh->mVertices[i];

        o_vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    for (auto i = 0; i < o_mesh->mNumFaces; i++)
    {
        const auto& face = o_mesh->mFaces[i];

        o_elements.push_back(face.mIndices[0]);
        o_elements.push_back(face.mIndices[1]);
        o_elements.push_back(face.mIndices[2]);
    }

    opengl::Buffer o_vbo;
    o_vbo.create();
    o_vbo.storage(core::buffer::make_data(o_vertices));

    opengl::Buffer o_ebo;
    o_ebo.create();
    o_ebo.storage(core::buffer::make_data(o_elements));

    opengl::VertexArray o_vao;
    o_vao.create();
    o_vao.attach_vertices(o_vbo, sizeof(glm::vec3));
    o_vao.attach_elements(o_ebo);

    o_vao.attribute(position_attribute);

    std::vector<glm::vec3> x_vertices;
    std::vector<uint32_t>  x_elements;

    for (auto i = 0; i < x_mesh->mNumVertices; i++)
    {
        const auto& vertex = x_mesh->mVertices[i];

        x_vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    for (auto i = 0; i < x_mesh->mNumFaces; i++)
    {
        const auto& face = x_mesh->mFaces[i];

        x_elements.emplace_back(face.mIndices[0]);
        x_elements.emplace_back(face.mIndices[1]);
        x_elements.emplace_back(face.mIndices[2]);
    }

    opengl::Buffer x_vbo;
    x_vbo.create();
    x_vbo.storage(core::buffer::make_data(x_vertices));

    opengl::Buffer x_ebo;
    x_ebo.create();
    x_ebo.storage(core::buffer::make_data(x_elements));

    opengl::VertexArray x_vao;
    x_vao.create();
    x_vao.attach_vertices(x_vbo, sizeof(glm::vec3));
    x_vao.attach_elements(x_ebo);

    x_vao.attribute(position_attribute);

    Assimp::Importer grid_importer;

    std::vector<glm::vec3> grid_vertices;
    std::vector<uint32_t>  grid_elements;

    const auto grid_scene = grid_importer.ReadFile("grid.obj", 0);
    const auto grid_mesh  = grid_scene->mMeshes[0];

    for (auto i = 0; i < grid_mesh->mNumVertices; i++)
    {
        const auto& vertex = grid_mesh->mVertices[i];

        grid_vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    for (auto i = 0; i < grid_mesh->mNumFaces; i++)
    {
        const auto& face = grid_mesh->mFaces[i];

        grid_elements.emplace_back(face.mIndices[0]);
        grid_elements.emplace_back(face.mIndices[1]);
        grid_elements.emplace_back(face.mIndices[2]);
    }

    opengl::Buffer grid_vbo;
    grid_vbo.create();
    grid_vbo.storage(core::buffer::make_data(grid_vertices));

    opengl::Buffer grid_ebo;
    grid_ebo.create();
    grid_ebo.storage(core::buffer::make_data(grid_elements));

    opengl::VertexArray grid_vao;
    grid_vao.create();
    grid_vao.attach_vertices(grid_vbo, sizeof(glm::vec3));
    grid_vao.attach_elements(grid_ebo);

    grid_vao.attribute(position_attribute);

    glm::mat4 model { 1.0f };

    glm::vec3 material_albedo { 1.0f };

    proj = glm::perspective(glm::radians(60.0f), static_cast<float>(window_width) / static_cast<float>(window_height), 0.1f, 100.0f);
    view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f));

    std::vector camera_uniforms
    {
        view, proj
    };

    opengl::Buffer transform_ubo;
    transform_ubo.create();
    transform_ubo.storage(core::buffer::make_data(&model), opengl::constants::dynamic_draw);
    transform_ubo.bind_base(opengl::constants::uniform_buffer, core::buffer::transform);

    opengl::Buffer camera_ubo;
    camera_ubo.create();
    camera_ubo.storage(core::buffer::make_data(camera_uniforms), opengl::constants::dynamic_draw);
    camera_ubo.bind_base(opengl::constants::uniform_buffer, core::buffer::camera);

    opengl::Buffer material_ubo;
    material_ubo.create();
    material_ubo.storage(core::buffer::make_data(&material_albedo), opengl::constants::dynamic_draw);
    material_ubo.bind_base(opengl::constants::uniform_buffer, core::buffer::material);

    constexpr auto tile_size = 1.5f;

    world = new btCollisionWorld(new btCollisionDispatcher(new btDefaultCollisionConfiguration()), new btDbvtBroadphase(), new btDefaultCollisionConfiguration());

    auto tile_shape = std::make_unique<btBoxShape>(btVector3(0.5f, 0.5f, 0.105f));

    for (auto row = 0; row < 3; row++)
    {
        for (auto col = 0; col < 3; col++)
        {
            const auto x = -tile_size + col * tile_size;
            const auto y =  tile_size - row * tile_size;

            btTransform transform;
            transform.setIdentity();
            transform.setOrigin(btVector3(x, y, 0.0f));

            auto tile_object = new btCollisionObject();
            tile_object->setCollisionShape(tile_shape.get());
            tile_object->setWorldTransform(transform);
            tile_object->setUserIndex(row);
            tile_object->setUserIndex2(col);

            world->addCollisionObject(tile_object);
        }
    }

    opengl::Pipeline::enable(opengl::constants::depth_test);
    opengl::Pipeline::enable(opengl::constants::cull_face);

    glm::vec3    x_color { 1.0f, 0.8392156862745098f, 0.22745098039215686f };
    glm::vec3    o_color { 0.9686274509803922f, 0.35294117647058826f, 0.35294117647058826f };
    glm::vec3 grid_color { 1.0f, 0.6627450980392157f, 0.3333333333333333f };

    while (!window_closed)
    {
        glfwPollEvents();

        opengl::Commands::clear(0.42745098039215684f, 0.8823529411764706f, 0.8235294117647058f, 1.0f);
        opengl::Commands::clear(opengl::constants::color_buffer | opengl::constants::depth_buffer);

        base_shader.bind();

        model = glm::mat4(1.0f);

        transform_ubo.update(core::buffer::make_data(&model));
         material_ubo.update(core::buffer::make_data(&grid_color));

        grid_vao.bind();

        opengl::Commands::draw_elements(opengl::constants::triangles, grid_elements.size());

        for (auto row = 0; row < 3; row++)
        {
            for (auto col = 0; col < 3; col++)
            {
                const auto x = -tile_size + col * tile_size;
                const auto y =  tile_size - row * tile_size;

                model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));

                transform_ubo.update(core::buffer::make_data(&model));

                if (tiles[row][col] == piece_x)
                {
                    material_ubo.update(core::buffer::make_data(&x_color));

                    x_vao.bind();

                    opengl::Commands::draw_elements(opengl::constants::triangles, x_elements.size());
                }
                else if (tiles[row][col] == piece_o)
                {
                    material_ubo.update(core::buffer::make_data(&o_color));

                    o_vao.bind();

                    opengl::Commands::draw_elements(opengl::constants::triangles, o_elements.size());
                }
            }
        }

        glfwSwapBuffers(window);
    }

    o_vao.destroy();
    o_vbo.destroy();
    o_ebo.destroy();

    x_vao.destroy();
    x_vbo.destroy();
    x_ebo.destroy();

    grid_vao.destroy();
    grid_vbo.destroy();
    grid_ebo.destroy();

    base_shader_vert.destroy();
    base_shader_frag.destroy();

    base_shader.destroy();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}