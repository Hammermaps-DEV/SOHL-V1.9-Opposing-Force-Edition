/*************************************************
*	Custom Vertex Array (CVA) Library
*
*	Copyright (c) Chain Studios, 2001-2005
*
*	This software is FREE and can be used and 
*	modified in any way. The user of this source
*	code should keep backwards compatibility with
*	ChainStudios' version of this library.
*************************************************/


#ifndef __CVA_H__
#define __CVA_H__


#define CVF_NUM_FLAGS			12

#define CV_VERTEX				0
#define CV_NORMAL				1
#define CV_PRIMARY_COLOR		2
#define CV_SECONDARY_COLOR		3
#define CV_COLOR				CV_PRIMARY_COLOR
#define CV_TEX0COORD2			4
#define CV_TEX1COORD2			5
#define CV_TEX2COORD2			6
#define CV_TEX3COORD2			7
#define CV_TEX0COORD3			8
#define CV_TEX1COORD3			9
#define CV_TEX2COORD3			10
#define CV_TEX3COORD3			11

#define CVF_VERTEX				(1<<CV_VERTEX)
#define CVF_NORMAL				(1<<CV_NORMAL)
#define CVF_PRIMARY_COLOR		(1<<CV_PRIMARY_COLOR)
#define CVF_SECONDARY_COLOR		(1<<CV_SECONDARY_COLOR)
#define CVF_COLOR				CVF_PRIMARY_COLOR
#define CVF_TEX0COORD2			(1<<CV_TEX0COORD2)
#define CVF_TEX1COORD2			(1<<CV_TEX1COORD2)
#define CVF_TEX2COORD2			(1<<CV_TEX2COORD2)
#define CVF_TEX3COORD2			(1<<CV_TEX3COORD2)
#define CVF_TEX0COORD3			(1<<CV_TEX0COORD3)
#define CVF_TEX1COORD3			(1<<CV_TEX1COORD3)
#define CVF_TEX2COORD3			(1<<CV_TEX2COORD3)
#define CVF_TEX3COORD3			(1<<CV_TEX3COORD3)

#define CVF_ALL					(1<<CVF_NUM_FLAGS)	//Cannot be used in init()

class cva
{
public:
	cva(int type, int count, int flags);
	cva(int type, int count, int flags, int elemsize, void *pmem);
	~cva();

	void	setoffsetof(int item, int offset);
	int		offsetof(int item);

	int		add(void *ptr, int size);
	void	modify(float f);
	bool	lock(void);
	bool	unlock(void);

	void	seek(int pos);
	void	rewind(void);
	int		tell(void);

	void	bind(int flags);
	void	unbind(void);

	void	draw(void);
	void	drawRange(int first, int count);
	void	drawTypeRange(int type, int first, int count);
 
private:
	float*	_data;
	int		_pos;
	int		_size;
	int		_totalsize;
	int		_ofs[CVF_NUM_FLAGS];
	int		_type;
	int		_items;
	int		_count;
	bool	_locked;
	bool	_bound;
	bool	_localmem;
};

extern "C"
{
bool cvaInit( void );
cva* cvaCreate( int type, int count, int flags );
cva* cvaCreateFromMemory( int type, int count, int flags, int elemsize, void *pmem );
void cvaDelete( cva* pcva );
}

#endif // __CVA_H__