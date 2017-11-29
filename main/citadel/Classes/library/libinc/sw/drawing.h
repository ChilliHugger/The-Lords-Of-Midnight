
#ifndef _DRAWING_H_INCLUDED
#define _DRAWING_H_INCLUDED

//#include <memory.h>

namespace chilli {

    using namespace chilli::types ;
    
	namespace lib {


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

        // size
        class  size
        {
        public:
            
            LONG cx;
            LONG cy;
            
            // Constructors
            size();
            size(int initCX, int initCY);
            size(point initPt);
            
            // Operations
            bool operator==(size) const;
            bool operator!=(size) const;
            void operator+=(size);
            void operator-=(size);
            
            // Operators returning size values
            size operator+(size) const;
            size operator-(size) const;
            size operator-() const;
            
            // Operators returning point values
            point operator+(point) const;
            point operator-(point) const;
            
            // Operators returning rect values
            rect operator+(const rect* lpRect) const;
            rect operator-(const rect* lpRect) const;
            
            //friend  archive& operator<<(archive& ar, const size&);
            //friend  archive& operator>>(archive& ar, size& );
            
            
        };
        
        /////////////////////////////////////////////////////////////////////////////
        // point - A 2-D point, similar to Windows point structure.
        
#define ONLEFT	0x01
#define ONTOP	0x02
        
        class  point //: public tagPOINT
        {
        public:
            
            LONG x;
            LONG y;
            
            // Constructors
            point();
            point(int initX, int initY);
            point(size initSize);
            
            // Operations
            void Offset(int xOffset, int yOffset);
            void Offset(point);
            void Offset(size);
            bool operator==(point) const;
            bool operator!=(point) const;
            void operator+=(size);
            void operator-=(size);
            void operator+=(point);
            void operator-=(point);
            
            // Operators returning point values
            point operator+(size) const;
            point operator-(size) const;
            point operator-() const;
            point operator+(point) const;
            
            // Operators returning size values
            size operator-(point) const;
            
            // Operators returning rect values
            rect operator+(const rect* lpRect) const;
            rect operator-(const rect* lpRect) const;
            
            friend  archive& operator<<(archive& ar, const point& );
            friend  archive& operator>>(archive& ar, point& );
            
            int PointPosition ( const point ) const;
            
        };
        
        /////////////////////////////////////////////////////////////////////////////
        // rect - A 2-D rectangle, similar to Windows RECT structure.
        
        
        class  rect //: public tagRECT
        {
        public:
            
            LONG left;
            LONG top;
            LONG right;
            LONG bottom;
            
            // Constructors
            rect();
            rect(int l, int t, int r, int b);
            rect(const rect& srrect);
            rect(const rect* lpSrrect);
            rect(point, size);
            rect(point topLeft, point bottomRight);
            
            // Attributes (in addition to RECT members)
            int Width() const;
            int Height() const;
            size Size() const;
            point& TopLeft();
            point& BottomRight();
            const point& TopLeft() const;
            const point& BottomRight() const;
            point CenterPoint() const;
            
            // convert between rect and LPRECT/LPrect (no need for &)
#ifdef LPRECT
            operator LPRECT();
            operator LPrect() const;
#endif
            
            bool IsRectEmpty() const;
            bool IsRectNull() const;
            bool PtInRect(point) const;
            
            int RectPosition ( const rect&  ) const;
            
            // Operations
            void SetRect(int x1, int y1, int x2, int y2);
            void SetRect(point topLeft, point bottomRight);
            void SetRectEmpty();
            void CopyRect(const rect* );
            bool EqualRect(const rect* ) const;
            
            void InflateRect(int x, int y);
            void InflateRect(size);
            void InflateRect(const rect* );
            void InflateRect(int l, int t, int r, int b);
            void DeflateRect(int x, int y);
            void DeflateRect(size);
            void DeflateRect(const rect* );
            void DeflateRect(int l, int t, int r, int b);
            
            void ClipRect(const rect* lpRect);
            
            void OffsetRect(int x, int y);
            void OffsetRect(size);
            void OffsetRect(point);
            void NormalizeRect();
            
            // operations that fill '*this' with result
            bool IntersectRect(const rect* , const rect* );
            bool UnionRect(const rect* , const rect* );
            
            // Additional Operations
            void operator=(const rect& );
            bool operator==(const rect& ) const;
            bool operator!=(const rect& ) const;
            void operator+=(point);
            void operator+=(size);
            void operator+=(const rect* );
            void operator-=(point);
            void operator-=(size);
            void operator-=(const rect* );
            void operator&=(const rect& );
            //	void operator|=(const rect& );
            
            // Operators returning rect values
            rect operator+(point) const;
            rect operator-(point) const;
            rect operator+(const rect* ) const;
            rect operator+(size) const;
            rect operator-(size) const;
            rect operator-(const rect* ) const;
            rect operator&(const rect& rect2) const;
            rect operator|(const rect& rect2) const;
            
            friend  archive& operator<<(archive& ar, const rect& );
            friend  archive& operator>>(archive& ar, rect& );
            
        };
        
        
		// size
		MXINLINE size::size()											{ /* filled */ }
		MXINLINE size::size(int initCX, int initCY)					{ cx = initCX; cy = initCY; }
		MXINLINE size::size(point initPt)								{ *(point*)this = initPt; }
		MXINLINE bool size::operator==(size value) const				{ return (cx == value.cx && cy == value.cy); }
		MXINLINE bool size::operator!=(size value) const				{ return (cx != value.cx || cy != value.cy); }
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
		MXINLINE bool point::operator==(point value) const			{ return (x == value.x && y == value.y); }
		MXINLINE bool point::operator!=(point value) const			{ return (x != value.x || y != value.y); }
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
		MXINLINE bool rect::IsRectEmpty() const						{ return ( (left >= right) || (top >= bottom) ); }
		MXINLINE bool rect::IsRectNull() const						{ return (left == 0 && right == 0 && top == 0 && bottom == 0); }
		MXINLINE void rect::OffsetRect(point point)					{ OffsetRect(point.x, point.y); }
		MXINLINE void rect::OffsetRect(size value)					{ OffsetRect( value.cx, value.cy); }
		MXINLINE void rect::SetRect(point topLeft, point bottomRight) { SetRect( topLeft.x, topLeft.y, bottomRight.x, bottomRight.y); }
		MXINLINE void rect::SetRectEmpty()							{ memset ( this, 0x00, sizeof( rect ) ); }
		MXINLINE void rect::CopyRect(const rect* lpSrcRect) 			{ memcpy ( this, lpSrcRect, sizeof( rect ) ); }
		MXINLINE void rect::operator=(const rect& srcRect)			{ CopyRect((const rect* )&srcRect); }
		MXINLINE bool rect::operator==(const rect& value) const		{ return EqualRect((const rect* )&value); }
		MXINLINE bool rect::operator!=(const rect& value) const		{ return !EqualRect((const rect* )&value); }
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


	}
	// namespace types


}
// namespace chilli

#endif // _DRAWING_H_INCLUDED
