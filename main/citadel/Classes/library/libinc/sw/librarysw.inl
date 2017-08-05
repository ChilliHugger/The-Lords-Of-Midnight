
#ifndef _LIBRARYSW_CPP_INCLUDED
#define _LIBRARYSW_CPP_INCLUDED

#include <memory.h>

#ifndef _USEINLINE_
	#define	MXINLINE
#else
	#define MXINLINE MXINLINE
#endif

using namespace chilli ;
using namespace chilli::types ;

namespace chilli {

	namespace lib {
		/* archive */
		MXINLINE BOOL archive::IsLoading() const						{ return (m_nMode & archive::load) != 0; }
		MXINLINE BOOL archive::IsStoring() const						{ return (m_nMode & archive::load) == 0; }
		MXINLINE BOOL archive::IsByteSwapping() const					{ return (m_nMode & archive::bByteSwap); }
		MXINLINE BOOL archive::IsBufferEmpty() const					{ return m_lpBufCur == m_lpBufMax; }
		MXINLINE ARFILE archive::GetFile() const					{ return m_pFile; }
		MXINLINE archive& archive::operator<<(int i)					{ return archive::operator<<((u32)i); }
		MXINLINE archive& archive::operator<<(short w)				{ return archive::operator<<((u16)w); }
		MXINLINE archive& archive::operator<<(char ch)				{ return archive::operator<<((BYTE)ch); }
		MXINLINE archive& archive::operator>>(int& i)					{ return archive::operator>>((u32&)i); }
		MXINLINE archive& archive::operator>>(short& w)				{ return archive::operator>>((u16&)w); }
		MXINLINE archive& archive::operator>>(char& ch)				{ return archive::operator>>((u8&)ch); }

		MXINLINE int Compare ( s32 number1, s32 number2 )
		{
			if ( number1<number2 ) return -1 ;
			if ( number1>number2 ) return 1 ;
			return 0;
		}

		MXINLINE int Compare ( f64 number1, f64 number2 )
		{
			if ( number1<number2 ) return -1 ;
			if ( number1>number2 ) return 1 ;
			return 0;
		}

		MXINLINE void swap ( int& num1, int& num2 )
		{
		int temp;
			temp = num1;
			num1=num2;
			num2=temp;
		}

		MXINLINE void swap ( u32& num1, u32& num2 )
		{
		u32 temp;
			temp = num1;
			num1=num2;
			num2=temp;
		}


		MXINLINE void swap ( u8& num1, u8& num2 )
		{
		u8 temp;
			temp = num1;
			num1=num2;
			num2=temp;
		}


	}
	// namespace lib

	namespace types {

		/* types::bitarray */
		MXINLINE BOOL bitarray::operator[](int bit) const				{ return Get(bit); }
		MXINLINE bitarray::bitarray()									{ Detatch(); }
		MXINLINE bitarray::bitarray ( int nElements )					{ Create (nElements ); }
		MXINLINE bitarray::~bitarray()								{ Destroy(); }
		MXINLINE void bitarray::ClrAll()								{ memset( m_pData, 0x00, Size() ); }
		MXINLINE void bitarray::SetAll()								{ memset( m_pData, 0xff, Size() ); }
		MXINLINE void bitarray::Attatch ( u8* pData, int nElements )	{ m_pData = pData; m_nElements = nElements ; }
		MXINLINE void bitarray::Detatch()								{ m_pData = NULL; m_nElements = 0 ;}
		MXINLINE void bitarray::Destroy()								{ if ( m_pData ) delete m_pData ;	m_pData = NULL;	m_nElements = 0; }
		MXINLINE chilli::lib::archive& operator<<( chilli::lib::archive& ar, bitarray& array )	{ return array.Serialize(ar); }
		MXINLINE chilli::lib::archive& operator>>( chilli::lib::archive& ar, bitarray& array )	{ return array.Serialize(ar); }

		/* flags32 */
		MXINLINE flags32::flags32()									{ m_flags = NONE ; }
		MXINLINE void flags32::Set ( u32 f )							{ m_flags |= f ; }
		MXINLINE void flags32::Reset ( u32 f )						{ m_flags &= ~f ; }
        MXINLINE void flags32::Toggle ( u32 f )                     { if ( Is(f) ) Reset(f); else Set(f); }
		MXINLINE BOOL flags32::Is ( u32 f) const						{ return m_flags&f ? TRUE : FALSE  ; }
		MXINLINE flags32::operator u32() const						{ return m_flags; }
		MXINLINE chilli::lib::archive& operator<<( chilli::lib::archive& ar, flags32& f )		{ return f.Serialize(ar); }
		MXINLINE chilli::lib::archive& operator>>( chilli::lib::archive& ar, flags32& f )		{ return f.Serialize(ar); }

		/* flags8 */
		MXINLINE flags8::flags8()										{ m_flags = NONE ; }
		MXINLINE void flags8::Set ( u8 f )							{ m_flags |= f ; }
		MXINLINE void flags8::Reset ( u8 f )							{ m_flags &= ~f ; }
        MXINLINE void flags8::Toggle ( u32 f )                     { if ( Is(f) ) Reset(f); else Set(f); }
		MXINLINE BOOL flags8::Is ( u8 f ) const						{ return m_flags&f ? TRUE : FALSE  ; }
		MXINLINE flags8::operator u8() const							{ return m_flags; }
		MXINLINE chilli::lib::archive& operator<<( chilli::lib::archive& ar, flags8& f )			{ return f.Serialize(ar); }
		MXINLINE chilli::lib::archive& operator>>( chilli::lib::archive& ar, flags8& f )		{ return f.Serialize(ar); }

		/* flags16 */
		MXINLINE flags16::flags16()									{ m_flags = NONE ; }
		MXINLINE void flags16::Set ( u16 f )							{ m_flags |= f ; }
		MXINLINE void flags16::Reset ( u16 f )						{ m_flags &= ~f ; }
        MXINLINE void flags16::Toggle ( u32 f )                     { if ( Is(f) ) Reset(f); else Set(f); }
		MXINLINE BOOL flags16::Is ( u16 f ) const						{ return m_flags&f ? TRUE : FALSE  ; }
		MXINLINE flags16::operator u16() const						{ return m_flags; }
		MXINLINE chilli::lib::archive& operator<<( chilli::lib::archive& ar, flags16& f )		{ return f.Serialize(ar); }
		MXINLINE chilli::lib::archive& operator>>( chilli::lib::archive& ar, flags16& f )		{ return f.Serialize(ar); }

		/* string */
		MXINLINE chilli::lib::archive& operator<<( chilli::lib::archive& ar, string& s )			{ return s.Serialize(ar); }
		MXINLINE chilli::lib::archive& operator>>( chilli::lib::archive& ar, string& s )		{ return s.Serialize(ar); }
		MXINLINE string::operator LPSTR() const						{ return m_pchData; }
		MXINLINE string::operator LPCSTR() const						{ return m_pchData; }

		// size
		MXINLINE size::size()											{ /* filled */ }
		MXINLINE size::size(int initCX, int initCY)					{ cx = initCX; cy = initCY; }
		MXINLINE size::size(point initPt)								{ *(point*)this = initPt; }
		MXINLINE BOOL size::operator==(size value) const				{ return (cx == value.cx && cy == value.cy); }
		MXINLINE BOOL size::operator!=(size value) const				{ return (cx != value.cx || cy != value.cy); }
		MXINLINE void size::operator+=(size value)					{ cx += value.cx; cy += value.cy; }
		MXINLINE void size::operator-=(size value)					{ cx -= value.cx; cy -= value.cy; }
		MXINLINE size size::operator+(size value) const				{ return size(cx + value.cx, cy + value.cy); }
		MXINLINE size size::operator-(size value) const				{ return size(cx - value.cx, cy - value.cy); }
		MXINLINE size size::operator-() const							{ return size(-cx, -cy); }
		MXINLINE point size::operator+(point value) const				{ return point(cx + value.x, cy + value.y); }
		MXINLINE point size::operator-(point value) const				{ return point(cx - value.x, cy - value.y); }

		// point
		MXINLINE point::point()										{ /* filled */ }
		MXINLINE point::point(int initX, int initY)					{ x = initX; y = initY; }
		MXINLINE point::point(size initSize)							{ *(size*)this = initSize; }
		MXINLINE void point::Offset(int xOffset, int yOffset)			{ x += xOffset; y += yOffset; }
		MXINLINE void point::Offset(point value)						{ x += value.x; y += value.y; }
		MXINLINE void point::Offset(size value)						{ x += value.cx; y += value.cy; }
		MXINLINE BOOL point::operator==(point value) const			{ return (x == value.x && y == value.y); }
		MXINLINE BOOL point::operator!=(point value) const			{ return (x != value.x || y != value.y); }
		MXINLINE void point::operator+=(size value)					{ x += value.cx; y += value.cy; }
		MXINLINE void point::operator-=(size value)					{ x -= value.cx; y -= value.cy; }
		MXINLINE void point::operator+=(point value)					{ x += value.x; y += value.y; }
		MXINLINE void point::operator-=(point value)					{ x -= value.x; y -= value.y; }
		MXINLINE point point::operator+(size value) const				{ return point(x + value.cx, y + value.cy); }
		MXINLINE point point::operator-(size value) const				{ return point(x - value.cx, y - value.cy); }
		MXINLINE point point::operator-() const						{ return point(-x, -y); }
		MXINLINE point point::operator+(point value) const			{ return point(x + value.x, y + value.y); }
		MXINLINE size point::operator-(point value) const				{ return size(x - value.x, y - value.y); }

		// rect
		MXINLINE rect::rect()											{ /* filled */ }
		MXINLINE rect::rect(int l, int t, int r, int b)				{ left = l; top = t; right = r; bottom = b; }
		MXINLINE rect::rect(const rect& srcRect)						{ CopyRect((const rect* )&srcRect); }
		MXINLINE rect::rect(const rect* lpSrcRect)					{ CopyRect(lpSrcRect); }
		MXINLINE rect::rect(point point, size size)					{ left = point.x; right = left + size.cx; top = point.y; bottom = top + size.cy; }
		MXINLINE rect::rect(point topLeft, point bottomRight)			{ left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }
		MXINLINE int rect::Width() const								{ return right - left; }
		MXINLINE int rect::Height() const								{ return bottom - top; }
		MXINLINE size rect::Size() const								{ return size(right - left, bottom - top); }
		MXINLINE point& rect::TopLeft()								{ return *((point*)this); }
		MXINLINE point& rect::BottomRight()							{ return *((point*)this+1); }
		MXINLINE const point& rect::TopLeft() const					{ return *((point*)this); }
		MXINLINE const point& rect::BottomRight() const				{ return *((point*)this+1); }
		MXINLINE point rect::CenterPoint() const						{ return point((left+right)/2, (top+bottom)/2); }
		//MXINLINE rect::operator LPRECT()							//	{ return (LPRECT)this; }
		//MXINLINE rect::operator LPCRECT() const						//	{ return (LPCRECT)this; }
		MXINLINE BOOL rect::IsRectEmpty() const						{ return ( (left >= right) || (top >= bottom) ); }
		MXINLINE BOOL rect::IsRectNull() const						{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
		MXINLINE void rect::OffsetRect(point point)					{ OffsetRect(point.x, point.y); }
		MXINLINE void rect::OffsetRect(size value)					{ OffsetRect( value.cx, value.cy); }
		MXINLINE void rect::SetRect(point topLeft, point bottomRight) { SetRect( topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
		MXINLINE void rect::SetRectEmpty()							{ memset ( this, 0x00, sizeof( rect ) ); }
		MXINLINE void rect::CopyRect(const rect* lpSrcRect) 			{ memcpy ( this, lpSrcRect, sizeof( rect ) ); }
		MXINLINE void rect::operator=(const rect& srcRect)			{ CopyRect((const rect* )&srcRect); }
		MXINLINE BOOL rect::operator==(const rect& value) const		{ return EqualRect((const rect* )&value); }
		MXINLINE BOOL rect::operator!=(const rect& value) const		{ return !EqualRect((const rect* )&value); }
		MXINLINE void rect::operator+=(point value)					{ OffsetRect(value.x, value.y); }
		MXINLINE void rect::operator+=(size value)					{ OffsetRect(value.cx, value.cy); }
		MXINLINE void rect::operator+=(const rect*  lpRect)			{ InflateRect(lpRect); }
		MXINLINE void rect::operator-=(point value)					{ OffsetRect(-value.x, -value.y); }
		MXINLINE void rect::operator-=(size value)					{ OffsetRect(-value.cx, -value.cy); }
		MXINLINE void rect::operator-=(const rect*  lpRect)			{ DeflateRect(lpRect); }
		MXINLINE void rect::operator&=(const rect& value)				{ IntersectRect((const rect* )this, (const rect* )&value); }
		MXINLINE rect rect::operator+(point pt) const					{ rect r(*this); r.OffsetRect( pt.x, pt.y); return r; }
		MXINLINE rect rect::operator-(point pt) const					{ rect r(*this); r.OffsetRect( -pt.x, -pt.y); return r; }
		MXINLINE rect rect::operator+(size s) const					{ rect r(*this); r.OffsetRect(s.cx, s.cy); return r; }
		MXINLINE rect rect::operator-(size s) const					{ rect r(*this); r.OffsetRect(-s.cx, -s.cy); return r; }

		// archive output helpers
		MXINLINE chilli::lib::archive& operator<<(chilli::lib::archive& ar, const size& s)		{ ar.Write(&s, sizeof(size)); return ar; }
		MXINLINE chilli::lib::archive& operator<<(chilli::lib::archive& ar, const point& p)		{ ar.Write(&p, sizeof(point)); return ar; }
		MXINLINE chilli::lib::archive& operator<<(chilli::lib::archive& ar, const rect& r)		{ ar.Write(&r, sizeof(rect)); return ar; }
		MXINLINE chilli::lib::archive& operator>>(chilli::lib::archive& ar, size& s)			{ ar.Read(&s, sizeof(size)); return ar; }
		MXINLINE chilli::lib::archive& operator>>(chilli::lib::archive& ar, point& p)			{ ar.Read(&p, sizeof(point)); return ar; }
		MXINLINE chilli::lib::archive& operator>>(chilli::lib::archive& ar, rect& r)			{ ar.Read(&r, sizeof(rect)); return ar; }
		MXINLINE chilli::lib::archive& operator<<(chilli::lib::archive& ar, const range& r)		{ ar.Write(&r, sizeof(range)); return ar; }
		MXINLINE chilli::lib::archive& operator>>(chilli::lib::archive& ar, range& r)				{ ar.Read(&r, sizeof(range)); return ar; }
	}
	// namespace types

#ifndef _DONT_DEFINE_MFC_CLASSES
	namespace os {
		/* os::file */
		MXINLINE file::file()											{ m_hFile = -1; m_bCloseOnDelete = FALSE; }
		MXINLINE file::file(int hFile)								{ m_hFile = hFile; m_bCloseOnDelete = FALSE; }
		MXINLINE file::file(LPCSTR lpszFileName, u32 nOpenFlags)		{ Open(lpszFileName, nOpenFlags); }
		MXINLINE file::~file()										{ if (IsValidHandle() && m_bCloseOnDelete) Close(); }
		MXINLINE LPSTR file::GetFileName()							{ return m_strFileName; }
		MXINLINE DWORD file::SeekToEnd()								{ return Seek(0, end); }
		MXINLINE void file::SeekToBegin()								{ Seek(0, begin); }
		MXINLINE BOOL file::IsOpen() const							{ return IsValidHandle(); }
		MXINLINE BOOL file::IsValidHandle() const						{ return m_hFile != -1; }
	}
#endif //_DONT_DEFINE_MFC_CLASSES


}
// namespace chilli

#endif // _LIBRARYSW_CPP_INCLUDED
