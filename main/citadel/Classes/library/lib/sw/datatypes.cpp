/*
 * FILE:    Datatypes.cpp
 * 
 * PROJECT: 
 * 
 * CREATED: 
 * 
 * AUTHOR:  Chris Wild
 * 
 * � Copyright 1999 The Midnight Council.
 * 
 * PURPOSE: 
 * 
 * 
 */


#include "../../libinc/library.h"
#include <cmath>

namespace chilli {

	namespace types {


		/////////////////////////////////////////////////////////////////////////////
		// out-of-line rect


		BOOL rect::PtInRect(point p) const
		{
			return ( (p.x >= left) && (p.x <= right) &&
					(p.y >= top) && (p.y <= bottom) );
		}

		void rect::SetRect(int x1, int y1, int x2, int y2)
		{
			left = x1 ;
			top = y1 ;
			right = x2 ;
			bottom = y2 ;
		}

		BOOL rect::EqualRect(const rect* lpRect) const
		{
			return ( (lpRect->left == left) && (lpRect->right == right) &&
					(lpRect->top == top) && (lpRect->bottom == bottom) );
		}

		void rect::NormalizeRect()
		{
		int nTemp;
			if (left > right) {
				nTemp = left;
				left = right;
				right = nTemp;
			}
			if (top > bottom) {
				nTemp = top;
				top = bottom;
				bottom = nTemp;
			}
		}


		/*
		void rect::ClipRect ( const rect& clip )
		{
			// clip the rectangles
			if ( right >= clip.right )
				right = clip.right;

			if ( left < clip.left )
				left = clip.left;

			if ( bottom >= clip.bottom )
				bottom = clip.bottom;

			if ( top < clip.top )
				top = clip.top;
		}
		*/


		void rect::InflateRect(const rect* lpRect)
		{
			left -= lpRect->left;
			top -= lpRect->top;
			right += lpRect->right;
			bottom += lpRect->bottom;
		}

		void rect::InflateRect(int l, int t, int r, int b)
		{
			left -= l;
			top -= t;
			right += r;
			bottom += b;
		}

		void rect::InflateRect(int x, int y )
		{
			left -= x;
			top -= y;
			right += x;
			bottom += y;
		}

		void rect::InflateRect( size s )
		{
			left -= s.cx;
			top -= s.cy;
			right += s.cx;
			bottom += s.cy;
		}

		void rect::DeflateRect(int x, int y )
		{
			left += x;
			top += y;
			right -= x;
			bottom -= y;
		}

		void rect::DeflateRect( size s )
		{
			left += s.cx;
			top += s.cy;
			right -= s.cx;
			bottom -= s.cy;
		}

		void rect::DeflateRect(const rect* lpRect)
		{
			left += lpRect->left;
			top += lpRect->top;
			right -= lpRect->right;
			bottom -= lpRect->bottom;
		}

		void rect::DeflateRect(int l, int t, int r, int b)
		{
			left += l;
			top += t;
			right -= r;
			bottom -= b;
		}

		void rect::OffsetRect(int x, int y)
		{
			left += x;
			right += x;
			top += y;
			bottom += y;

		}

		int rect::RectPosition ( const rect& r ) const
		{
			point point1 = CenterPoint();
			point point2 = r.CenterPoint();
			return point2.PointPosition( point1 );
		}

		int point::PointPosition ( const point p ) const
		{
		int temp=0;
			if ( x < p.x )	temp |= ONLEFT;
			if ( y < p.y )	temp |= ONTOP;
			return temp;
		}



		BOOL rect::IntersectRect(const rect* lpRect1, const rect* lpRect2)
		{
			SetRectEmpty();

			if ( ((rect)lpRect2).IsRectEmpty() || ((rect)lpRect2).IsRectEmpty() )
				return ( FALSE );

			if ( lpRect1->left > lpRect2->right )
				return ( FALSE );

			if ( lpRect1->right < lpRect2->left )
				return ( FALSE );

			if ( lpRect1->top > lpRect2->bottom )
				return ( FALSE );

			if ( lpRect1->bottom < lpRect2->top )
				return ( FALSE );


			left = MAX( lpRect1->left, lpRect2->left );
			right = MIN( lpRect1->right, lpRect2->right );
			top = MAX( lpRect1->top, lpRect2->top );
			bottom = MIN( lpRect1->bottom, lpRect2->bottom );

			return ( TRUE );
		}




		BOOL rect::UnionRect( const rect* lpRect1, const rect* lpRect2)
		{
			SetRectEmpty();

			if ( lpRect1->IsRectEmpty() || lpRect2->IsRectEmpty() )
				return ( FALSE );

			left = MIN( lpRect1->left, lpRect2->left );
			right = MAX( lpRect1->right, lpRect2->right );
			top = MIN( lpRect1->top, lpRect2->top );
			bottom = MAX( lpRect1->bottom, lpRect2->bottom );

			return ( TRUE );
		}



		void rect::ClipRect ( const rect* r )
		{
		/* clip left/right */


			if ( left < r->left )
				left = r->left;
			else if ( left > r->right ) {
				left = r->right;
				return;
			}

			if ( right > r->right )
				right = r->right;
			else if ( right < r->left ) {
				right = r->left;
				return;
			}

		/* clip top/bottom */
			if ( top < r->top )
				top = r->top;
			else if ( top > r->bottom ) {
				left = r->bottom;
				return;
			}

			if ( bottom > r->bottom )
				bottom = r->bottom;
			else if ( bottom < r->top ) {
				bottom = r->top;
			}

		}




		empty::empty()
		{
		}

		empty::~empty()
		{
		}


	}
	// namespace types
}
// namespace tme
