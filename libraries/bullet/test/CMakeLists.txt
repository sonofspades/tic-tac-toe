IF(BUILD_BULLET3)
	#SUBDIRS( TestBullet3OpenCL )
	SUBDIRS(  InverseDynamics SharedMemory )
ENDIF(BUILD_BULLET3)

SUBDIRS(  collision BulletDynamics )

