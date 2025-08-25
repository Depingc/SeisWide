//---------------------------------------------------------------------------
// 
// CStringEx::Scanf extension                         (c) Rainer Bauer, 2001
//                                                       codeguru@jonkks.com
// 
// You can use/modify these functions as you wish.
// 
// Extends Zafir Anjum's CStringEx class by function Scanf(). CStringEx
// can be found at http://www.codeguru.com/string/cstring_ext.shtml
// 
// Scanf() will scan the string stored in the CStringEx object like
// function sscanf() would do.
// 
// Example:
// 
//   int        i;
//   CStringEx  s("100");
// 
//   sscanf( s, "%d", &i );   // returns no. of assigned itmes
//   s.Scanf( "%d", &i );     // returns no. of assigned itmes
//   s.Scanf( 1, "%d", &i );  // returns TRUE, if all itmes were assigned
// 
// Note: Source formated with tab width "3" and indent "3".
// 
//---------------------------------------------------------------------------

#ifndef __STRINGEX_H_
#define __STRINGEX_H_

class CStringEx : public CString
{
public:

	//------------------------------------------------------------------------

	CStringEx()										: CString( ){};
	CStringEx( const CString& stringSrc)	: CString( stringSrc ){};
	CStringEx( const CStringEx& stringSrc)	: CString( stringSrc ){};
	CStringEx( TCHAR ch, int nRepeat = 1 )	: CString( ch, nRepeat ){};
	CStringEx( LPCTSTR lpch, int nLength )	: CString( lpch, nLength ){};
	CStringEx( const unsigned char* psz )	: CString( psz ){};
	CStringEx( LPCWSTR lpsz )					: CString( lpsz ){};
	CStringEx( LPCSTR lpsz )					: CString( lpsz ){};

	//------------------------------------------------------------------------
	// Extension starts here !
	//------------------------------------------------------------------------

	BOOL	Scanf( int numArgs, LPCTSTR format, ... );
	int	Scanf( LPCTSTR format, ... );

private:

	int	sscanfWrapper( int numArgs, LPCTSTR format, void **p );
};


//---------------------------------------------------------------------------

// Note: when increasing no. of possible arguments,
// the switch statement in functions Scanf() must be extened!
#define CSTRINGEX_SCANF_MAX_ARGS	10

// Define some macros for readability
//
// Does anyone know how these macros can be replaced by one single macro ???
//
#define ADD_ARGS_1  p[0]
#define ADD_ARGS_2  p[0], p[1]
#define ADD_ARGS_3  p[0], p[1], p[2]
#define ADD_ARGS_4  p[0], p[1], p[2], p[3]
#define ADD_ARGS_5  p[0], p[1], p[2], p[3], p[4]
#define ADD_ARGS_6  p[0], p[1], p[2], p[3], p[4], p[5]
#define ADD_ARGS_7  p[0], p[1], p[2], p[3], p[4], p[5], p[6]
#define ADD_ARGS_8  p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]
#define ADD_ARGS_9  p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8]
#define ADD_ARGS_10 p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9]


//---------------------------------------------------------------------------
int CStringEx::sscanfWrapper( int numArgs, LPCTSTR format, void **p )
{
	switch ( numArgs )
	{
	case  0: return 0;
	case  1: return _stscanf( m_pchData, format, ADD_ARGS_1  );
	case  2: return _stscanf( m_pchData, format, ADD_ARGS_2  );
	case  3: return _stscanf( m_pchData, format, ADD_ARGS_3  );
	case  4: return _stscanf( m_pchData, format, ADD_ARGS_4  );
	case  5: return _stscanf( m_pchData, format, ADD_ARGS_5  );
	case  6: return _stscanf( m_pchData, format, ADD_ARGS_6  );
	case  7: return _stscanf( m_pchData, format, ADD_ARGS_7  );
	case  8: return _stscanf( m_pchData, format, ADD_ARGS_8  );
	case  9: return _stscanf( m_pchData, format, ADD_ARGS_9  );
	case 10: return _stscanf( m_pchData, format, ADD_ARGS_10 );
	}

	// When extending max. no. of arguments [CSTRINGEX_SCANF_MAX_ARGS],
	// this function must be updated!
	ASSERT( !TRUE );
	return 0;
}


//---------------------------------------------------------------------------
BOOL CStringEx::Scanf( int numArgs, LPCTSTR format, ... )
{
	int			i;
	int			numScanned;
	void			*argPointer[CSTRINGEX_SCANF_MAX_ARGS];
	va_list		arglist;
	
	if ( numArgs > CSTRINGEX_SCANF_MAX_ARGS )
	{
		// This function can only handle <CSTRINGEX_SCANF_MAX_ARGS> (10) arguments!
		ASSERT( !TRUE );
		return FALSE;
	}

	va_start( arglist, format );
	{
		// Get all arguments from stack
		for ( i=0; i<numArgs; i++ )
		{
			argPointer[i] = va_arg( arglist, void * );
		}
	}
	va_end( arglist );
	
	// Call sscanf with correct no. of arguments
	numScanned = sscanfWrapper( numArgs, format, argPointer );

	return (numScanned == numArgs);
}


//---------------------------------------------------------------------------
int CStringEx::Scanf( LPCTSTR format, ... )
{
	int			numArgs;
	int			numScanned;
	void			*argPointer[CSTRINGEX_SCANF_MAX_ARGS];
	va_list		arglist;
	LPTSTR		currBuff;
	_TXCHAR		currChar;

	numArgs	= 0;
	currBuff = _tcschr( format, _TXCHAR( '%' ) );
	if ( currBuff == NULL )
	{
		// No valid format specifier!
		ASSERT( !TRUE );
		return 0;
	}
	
	va_start( arglist, format );
	{
		do
		{
			// Move pointer to next character
			currBuff = _tcsinc( currBuff );
			currChar = _TXCHAR( *currBuff );
			
			if ( currChar == NULL )
			{
				// End of string
				//      -> processing will stop!
			}
			else if ( currChar == _TXCHAR( '*' ) )
			{
				// "%*" suppresses argument assignment
				//      -> do not get argument from stack!
			}
			else if ( currChar == _TXCHAR( '%' ) )
			{
				// "%%" substitutes "%" character!
				//      -> do not get argument from stack!
				//      -> Increment to next character
				
				currBuff = _tcsinc( currBuff );
			}
			else
			{
				if ( numArgs >= CSTRINGEX_SCANF_MAX_ARGS )
				{
					// This function can only handle
					// <CSTRINGEX_SCANF_MAX_ARGS> (10) arguments!
					ASSERT( !TRUE );
					return 0;
				}
				
				argPointer[numArgs++] = va_arg( arglist, void * );
			}
			
			currBuff = _tcschr( currBuff, _TXCHAR( '%' ) );
			
		} while ( currBuff != NULL );
	}
	va_end( arglist );
	
	// Call sscanf with correct no. of arguments
	numScanned = sscanfWrapper( numArgs, format, argPointer );

	return numScanned;
}

#endif // __STRINGEX_H_
