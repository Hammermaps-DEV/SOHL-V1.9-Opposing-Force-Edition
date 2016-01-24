/******************************************
* userdata.h                              *
* Definition of extra user variables      *
******************************************/

// These flags let the client know what user data is being sent
#define FL_UPDATEUSERDATA_NEWVECTOR_X (1<<0)
#define FL_UPDATEUSERDATA_NEWVECTOR_Y (1<<1)
#define FL_UPDATEUSERDATA_NEWVECTOR_Z (1<<2)
#define FL_UPDATEUSERDATA_FUEL        (1<<3)

#define MAX_FUEL 1000

typedef struct {
	BOOL CanWallJump;
	BOOL CanDoubleJump;
	float JumpHeight;
} user_variables_t;