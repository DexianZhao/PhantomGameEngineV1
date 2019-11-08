/*
www.sourceforge.net/projects/tinyxml
Original code (2.0 and earlier )copyright (c) 2000-2002 Lee Thomason (www.grinninglizard.com)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include <ctype.h>
#include "tinyxmlw.h"

const wchar_t* TiXmlBaseW::errorString[ TIXML_ERROR_STRING_COUNT ] =
{
	L"No error",
	L"Error",
	L"Failed to open file",
	L"Memory allocation failed.",
	L"Error parsing Element.",
	L"Failed to read Element name",
	L"Error reading Element value.",
	L"Error reading Attributes.",
	L"Error: empty tag.",
	L"Error reading end tag.",
	L"Error parsing Unknown.",
	L"Error parsing Comment.",
	L"Error parsing Declaration.",
	L"Error document empty."
};

bool TiXmlBaseW::condenseWhiteSpace = true;

void TiXmlBaseW::PutString( const TIXML_STRING_W& str, TIXML_OSTREAM_W* stream )
{
	TIXML_STRING_W buffer;
	PutString( str, &buffer );
	(*stream) << buffer;
}

void TiXmlBaseW::PutString( const TIXML_STRING_W& str, TIXML_STRING_W* outString )
{
	int i=0;

	while( i<(int)str.length() )
	{
		int c = str[i];

		if (    c == L'&' 
		     && i < ( (int)str.length() - 2 )
			 && str[i+1] == L'#'
			 && str[i+2] == L'x' )
		{
			// Hexadecimal character reference.
			// Pass through unchanged.
			// &#xA9;	-- copyright symbol, for example.
			while ( i<(int)str.length() )
			{
				outString->append( str.c_str() + i, 1 );
				++i;
				if ( str[i] == L';' )
					break;
			}
		}
		else if ( c == L'&' )
		{
			outString->append( entity[0].str, entity[0].strLength );
			++i;
		}
		else if ( c == L'<' )
		{
			outString->append( entity[1].str, entity[1].strLength );
			++i;
		}
		else if ( c == L'>' )
		{
			outString->append( entity[2].str, entity[2].strLength );
			++i;
		}
		else if ( c == L'\"' )
		{
			outString->append( entity[3].str, entity[3].strLength );
			++i;
		}
		else if ( c == L'\'' )
		{
			outString->append( entity[4].str, entity[4].strLength );
			++i;
		}
		else if ( c < 32 || c > 126 )
		{
			// Easy pass at non-alpha/numeric/symbol
			// 127 is the delete key. Below 32 is symbolic.
			wchar_t buf[ 32 ];
			swprintf( buf, L"&#x%02X;", (unsigned) ( c & 0xff ) );
			outString->append( buf, (int)wcslen( buf ) );
			++i;
		}
		else
		{
			wchar_t realc = (wchar_t) c;
			outString->append( &realc, 1 );
			++i;
		}
	}
}


TiXmlNodeW::TiXmlNodeW( NodeType _type )
{
	parent = 0;
	type = _type;
	firstChild = 0;
	lastChild = 0;
	prev = 0;
	next = 0;
	userData = 0;
}


TiXmlNodeW::~TiXmlNodeW()
{
	TiXmlNodeW* node = firstChild;
	TiXmlNodeW* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	
}


void TiXmlNodeW::Clear()
{
	TiXmlNodeW* node = firstChild;
	TiXmlNodeW* temp = 0;

	while ( node )
	{
		temp = node;
		node = node->next;
		delete temp;
	}	

	firstChild = 0;
	lastChild = 0;
}


TiXmlNodeW* TiXmlNodeW::LinkEndChild( TiXmlNodeW* node )
{
	node->parent = this;

	node->prev = lastChild;
	node->next = 0;

	if ( lastChild )
		lastChild->next = node;
	else
		firstChild = node;			// it was an empty list.

	lastChild = node;
	return node;
}


TiXmlNodeW* TiXmlNodeW::InsertEndChild( const TiXmlNodeW& addThis )
{
	TiXmlNodeW* node = addThis.Clone();
	if ( !node )
		return 0;

	return LinkEndChild( node );
}


TiXmlNodeW* TiXmlNodeW::InsertBeforeChild( TiXmlNodeW* beforeThis, const TiXmlNodeW& addThis )
{	
	if ( !beforeThis || beforeThis->parent != this )
		return 0;

	TiXmlNodeW* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->next = beforeThis;
	node->prev = beforeThis->prev;
	if ( beforeThis->prev )
	{
		beforeThis->prev->next = node;
	}
	else
	{
		assert( firstChild == beforeThis );
		firstChild = node;
	}
	beforeThis->prev = node;
	return node;
}


TiXmlNodeW* TiXmlNodeW::InsertAfterChild( TiXmlNodeW* afterThis, const TiXmlNodeW& addThis )
{
	if ( !afterThis || afterThis->parent != this )
		return 0;

	TiXmlNodeW* node = addThis.Clone();
	if ( !node )
		return 0;
	node->parent = this;

	node->prev = afterThis;
	node->next = afterThis->next;
	if ( afterThis->next )
	{
		afterThis->next->prev = node;
	}
	else
	{
		assert( lastChild == afterThis );
		lastChild = node;
	}
	afterThis->next = node;
	return node;
}


TiXmlNodeW* TiXmlNodeW::ReplaceChild( TiXmlNodeW* replaceThis, const TiXmlNodeW& withThis )
{
	if ( replaceThis->parent != this )
		return 0;

	TiXmlNodeW* node = withThis.Clone();
	if ( !node )
		return 0;

	node->next = replaceThis->next;
	node->prev = replaceThis->prev;

	if ( replaceThis->next )
		replaceThis->next->prev = node;
	else
		lastChild = node;

	if ( replaceThis->prev )
		replaceThis->prev->next = node;
	else
		firstChild = node;

	delete replaceThis;
	node->parent = this;
	return node;
}


bool TiXmlNodeW::RemoveChild( TiXmlNodeW* removeThis )
{
	if ( removeThis->parent != this )
	{	
		assert( 0 );
		return false;
	}

	if ( removeThis->next )
		removeThis->next->prev = removeThis->prev;
	else
		lastChild = removeThis->prev;

	if ( removeThis->prev )
		removeThis->prev->next = removeThis->next;
	else
		firstChild = removeThis->next;

	delete removeThis;
	return true;
}

TiXmlNodeW* TiXmlNodeW::FirstChild( const wchar_t * value ) const
{
	TiXmlNodeW* node;
	for ( node = firstChild; node; node = node->next )
	{
		if ( node->SValue() == TIXML_STRING_W( value ))
			return node;
	}
	return 0;
}

TiXmlNodeW* TiXmlNodeW::LastChild( const wchar_t * value ) const
{
	TiXmlNodeW* node;
	for ( node = lastChild; node; node = node->prev )
	{
		if ( node->SValue() == TIXML_STRING_W (value))
			return node;
	}
	return 0;
}

TiXmlNodeW* TiXmlNodeW::IterateChildren( TiXmlNodeW* previous ) const
{
	if ( !previous )
	{
		return FirstChild();
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling();
	}
}

TiXmlNodeW* TiXmlNodeW::IterateChildren( const wchar_t * val, TiXmlNodeW* previous ) const
{
	if ( !previous )
	{
		return FirstChild( val );
	}
	else
	{
		assert( previous->parent == this );
		return previous->NextSibling( val );
	}
}

TiXmlNodeW* TiXmlNodeW::NextSibling( const wchar_t * value ) const
{
	TiXmlNodeW* node;
	for ( node = next; node; node = node->next )
	{
		if ( node->SValue() == TIXML_STRING_W (value))
			return node;
	}
	return 0;
}


TiXmlNodeW* TiXmlNodeW::PreviousSibling( const wchar_t * value ) const
{
	TiXmlNodeW* node;
	for ( node = prev; node; node = node->prev )
	{
		if ( node->SValue() == TIXML_STRING_W (value))
			return node;
	}
	return 0;
}

void TiXmlElementW::RemoveAttribute( const wchar_t * name )
{
	TiXmlAttributeW* node = attributeSet.Find( name );
	if ( node )
	{
		attributeSet.Remove( node );
		delete node;
	}
}

TiXmlElementW* TiXmlNodeW::FirstChildElement() const
{
	TiXmlNodeW* node;

	for (	node = FirstChild();
	node;
	node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

TiXmlElementW* TiXmlNodeW::FirstChildElement( const wchar_t * value ) const
{
	TiXmlNodeW* node;

	for (	node = FirstChild( value );
	node;
	node = node->NextSibling( value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}


TiXmlElementW* TiXmlNodeW::NextSiblingElement() const
{
	TiXmlNodeW* node;

	for (	node = NextSibling();
	node;
	node = node->NextSibling() )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}

TiXmlElementW* TiXmlNodeW::NextSiblingElement( const wchar_t * value ) const
{
	TiXmlNodeW* node;

	for (	node = NextSibling( value );
	node;
	node = node->NextSibling( value ) )
	{
		if ( node->ToElement() )
			return node->ToElement();
	}
	return 0;
}



TiXmlDocumentW* TiXmlNodeW::GetDocument() const
{
	const TiXmlNodeW* node;

	for( node = this; node; node = node->parent )
	{
		if ( node->ToDocument() )
			return node->ToDocument();
	}
	return 0;
}


TiXmlElementW::TiXmlElementW (const wchar_t * _value)
: TiXmlNodeW( TiXmlNodeW::ELEMENT )
{
	firstChild = lastChild = 0;
	value = _value;
}

TiXmlElementW::~TiXmlElementW()
{
	while( attributeSet.First() )
	{
		TiXmlAttributeW* node = attributeSet.First();
		attributeSet.Remove( node );
		delete node;
	}
}

// BEGIN OGRE CHANGES
// USE STL ALONE
#ifndef TIXML_USE_STL
const wchar_t * TiXmlElementW::Attribute( const wchar_t * name ) const
{
	TiXmlAttributeW* node = attributeSet.Find( name );

	if ( node )
		return node->Value();

	return 0;
}


const wchar_t * TiXmlElementW::Attribute( const wchar_t * name, int* i ) const
{
	const wchar_t * s = Attribute( name );
	if ( i )
	{
		if ( s )
			*i = _wtoi( s );
		else
			*i = 0;
	}
	return s;
}

const wchar_t* TiXmlElementW::Attribute( const wchar_t* name, float* f) const
{
	const wchar_t * s = Attribute( name );
	if ( f )
	{
		if ( s )
			*f = (float)_wtof( s );
		else
			*f = 0;
	}
	return s;
}

void TiXmlElementW::SetAttribute( const wchar_t * name, int val )
{	
	wchar_t buf[64];
	swprintf( buf, L"%d", val );
	SetAttribute( name, buf );
}


void TiXmlElementW::SetAttribute( const wchar_t * name, const wchar_t * value )
{
	TiXmlAttributeW* node = attributeSet.Find( name );
	if ( node )
	{
		node->SetValue( value );
		return;
	}

	TiXmlAttributeW* attrib = new TiXmlAttributeW( name, value );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		TiXmlDocumentW* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY );
	}
}

#endif

#ifdef TIXML_USE_STL

const wchar_t * TiXmlElementW::Attribute( const std::wstring& name ) const
{
	TiXmlAttributeW* node = attributeSet.Find( name.c_str() );

	if ( node )
		return node->Value();

	return 0;
}


const wchar_t * TiXmlElementW::Attribute( const std::wstring& name, int* i ) const
{
	const wchar_t * s = Attribute( name );
	if ( i )
	{
		if ( s )
			*i = _wtoi( s );
		else
			*i = 0;
	}
	return s;
}


void TiXmlElementW::SetAttribute( const std::wstring& name, int val )
{	
	wchar_t buf[64];
	swprintf( buf, L"%d", val );
	SetAttribute( name, buf );
}


void TiXmlElementW::SetAttribute( const std::wstring& name, const std::wstring& value )
{
	TiXmlAttributeW* node = attributeSet.Find( name.c_str() );
	if ( node )
	{
		node->SetValue( value );
		return;
	}

	TiXmlAttributeW* attrib = new TiXmlAttributeW( name, value );
	if ( attrib )
	{
		attributeSet.Add( attrib );
	}
	else
	{
		TiXmlDocumentW* document = GetDocument();
		if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY );
	}
}

#endif
// END OGRE CHANGES
void TiXmlElementW::Print( FILE* cfile, int depth ) const
{
	int i;
	for ( i=0; i<depth; i++ )
	{
		fwprintf( cfile, L"    " );
	}

	fwprintf( cfile, L"<%s", value.c_str() );

	TiXmlAttributeW* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{
		fwprintf( cfile, L" " );
		attrib->Print( cfile, depth );
	}

	// There are 3 different formatting approaches:
	// 1) An element without children is printed as a <foo /> node
	// 2) An element with only a text child is printed as <foo> text </foo>
	// 3) An element with children is printed on multiple lines.
	TiXmlNodeW* node;
	if ( !firstChild )
	{
		fwprintf( cfile, L" />" );
	}
	else if ( firstChild == lastChild && firstChild->ToText() )
	{
		fwprintf( cfile, L">" );
		firstChild->Print( cfile, depth + 1 );
		fwprintf( cfile, L"</%s>", value.c_str() );
	}
	else
	{
		fwprintf( cfile, L">" );

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			if ( !node->ToText() )
			{
				fwprintf( cfile, L"\n" );
			}
			node->Print( cfile, depth+1 );
		}
		fwprintf( cfile, L"\n" );
		for( i=0; i<depth; ++i )
		fwprintf( cfile, L"    " );
		fwprintf( cfile, L"</%s>", value.c_str() );
	}
}

void TiXmlElementW::StreamOut( TIXML_OSTREAM_W * stream ) const
{
	(*stream) << L"<" << value;

	TiXmlAttributeW* attrib;
	for ( attrib = attributeSet.First(); attrib; attrib = attrib->Next() )
	{	
		(*stream) << L" ";
		attrib->StreamOut( stream );
	}

	// If this node has children, give it a closing tag. Else
	// make it an empty tag.
	TiXmlNodeW* node;
	if ( firstChild )
	{ 		
		(*stream) << L">";

		for ( node = firstChild; node; node=node->NextSibling() )
		{
			node->StreamOut( stream );
		}
		(*stream) << L"</" << value << L">";
	}
	else
	{
		(*stream) << L" />";
	}
}

TiXmlNodeW* TiXmlElementW::Clone() const
{
	TiXmlElementW* clone = new TiXmlElementW( Value() );
	if ( !clone )
		return 0;

	CopyToClone( clone );

	// Clone the attributes, then clone the children.
	TiXmlAttributeW* attribute = 0;
	for(	attribute = attributeSet.First();
	attribute;
	attribute = attribute->Next() )
	{
		clone->SetAttribute( attribute->Name(), attribute->Value() );
	}

	TiXmlNodeW* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->LinkEndChild( node->Clone() );
	}
	return clone;
}


TiXmlDocumentW::TiXmlDocumentW() : TiXmlNodeW( TiXmlNodeW::DOCUMENT )
{
	error = false;
	//	ignoreWhiteSpace = true;
}

TiXmlDocumentW::TiXmlDocumentW( const wchar_t * documentName ) : TiXmlNodeW( TiXmlNodeW::DOCUMENT )
{
	//	ignoreWhiteSpace = true;
	value = documentName;
	error = false;
}

bool TiXmlDocumentW::LoadFile()
{
	if ( LoadFile( value.c_str() ) )
		return true;

	return false;
}


bool TiXmlDocumentW::SaveFile( const wchar_t* szHeadText ) const
{
	if ( SaveFile( value.c_str() ,szHeadText ) )
		return true;

	return false;
}

bool TiXmlDocumentW::LoadFromMemory(const wchar_t* szText)
{
	// Delete the existing data:
	Clear();
	//
	Parse(szText);
	if (  !Error() )
	{
		return true;
	}
	SetError( TIXML_ERROR_OPENING_FILE );
	return false;
}

bool TiXmlDocumentW::LoadFile( const wchar_t* filename )
{
	// Delete the existing data:
	Clear();

	// There was a really terrifying little bug here. The code:
	//		value = filename
	// in the STL case, cause the assignment method of the std::wstring to
	// be called. What is strange, is that the std::wstring had the same
	// address as it's c_str() method, and so bad things happen. Looks
	// like a bug in the Microsoft STL implementation.
	// See STL_STRING_BUG above.
	// Fixed with the StringToBuffer class.
	value = filename;

	FILE* file = _wfopen( value.c_str (), L"r" );

	if ( file )
	{
		// Get the file size, so we can pre-allocate the wstring. HUGE speed impact.
		long length = 0;
		fseek( file, 0, SEEK_END );
		length = ftell( file );
		fseek( file, 0, SEEK_SET );

		// Strange case, but good to handle up front.
		if ( length == 0 )
		{
			fclose( file );
			return false;
		}

		// If we have a file, assume it is all one big XML file, and read it in.
		// The document parser may decide the document ends sooner than the entire file, however.
		TIXML_STRING_W data;
		data.reserve( length );

		const int BUF_SIZE = 2048;
		wchar_t buf[BUF_SIZE];
		
		while( fgetws( buf, BUF_SIZE, file ) )
		{
			data += buf;
		}
		fclose( file );

		Parse( data.c_str() );
		if (  !Error() )
		{
			return true;
		}
	}
	SetError( TIXML_ERROR_OPENING_FILE );
	return false;
}

bool TiXmlDocumentW::SaveFile( const wchar_t * filename , const wchar_t* szHeadText) const
{
	// The old c stuff lives on...
	FILE* fp = _wfopen( filename, L"w" );
	if ( fp )
	{
		if(!szHeadText)
		{
			const wchar_t * szHead = L"<xml>\r\n";
			fwprintf(fp,szHead);
		}
		//
		Print( fp, 0 );
		//
		if(!szHeadText)
		{
			const wchar_t * szHead = L"\r\n</xml>";
			fwprintf(fp,szHead);
		}
		//
		fclose( fp );
		return true;
	}
	return false;
}


TiXmlNodeW* TiXmlDocumentW::Clone() const
{
	TiXmlDocumentW* clone = new TiXmlDocumentW();
	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->error = error;
	clone->errorDesc = errorDesc.c_str ();

	TiXmlNodeW* node = 0;
	for ( node = firstChild; node; node = node->NextSibling() )
	{
		clone->LinkEndChild( node->Clone() );
	}
	return clone;
}


void TiXmlDocumentW::Print( FILE* cfile, int depth ) const
{
	TiXmlNodeW* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{
		node->Print( cfile, depth );
		fwprintf( cfile, L"\n" );
	}
}

void TiXmlDocumentW::StreamOut( TIXML_OSTREAM_W * out ) const
{
	TiXmlNodeW* node;
	for ( node=FirstChild(); node; node=node->NextSibling() )
	{
		node->StreamOut( out );

		// Special rule for streams: stop after the root element.
		// The stream in code will only read one element, so don't
		// write more than one.
		if ( node->ToElement() )
			break;
	}
}

TiXmlAttributeW* TiXmlAttributeW::Next() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( next->value.empty() && next->name.empty() )
		return 0;
	return next;
}


TiXmlAttributeW* TiXmlAttributeW::Previous() const
{
	// We are using knowledge of the sentinel. The sentinel
	// have a value or name.
	if ( prev->value.empty() && prev->name.empty() )
		return 0;
	return prev;
}


void TiXmlAttributeW::Print( FILE* cfile, int /*depth*/ ) const
{
	TIXML_STRING_W n, v;

	PutString( Name(), &n );
	PutString( Value(), &v );

	if (value.find (L'\"') == TIXML_STRING_W::npos)
		fwprintf (cfile, L"%s=\"%s\"", n.c_str(), v.c_str() );
	else
		fwprintf (cfile, L"%s='%s'", n.c_str(), v.c_str() );
}


void TiXmlAttributeW::StreamOut( TIXML_OSTREAM_W * stream ) const
{
	if (value.find( L'\"' ) != TIXML_STRING_W::npos)
	{
		PutString( name, stream );
		(*stream) << L"=" << L"'";
		PutString( value, stream );
		(*stream) << L"'";
	}
	else
	{
		PutString( name, stream );
		(*stream) << L"=" << L"\"";
		PutString( value, stream );
		(*stream) << L"\"";
	}
}

void TiXmlAttributeW::SetIntValue( int value )
{
	wchar_t buf [64];
	swprintf (buf, L"%d", value);
	SetValue (buf);
}

void TiXmlAttributeW::SetDoubleValue( double value )
{
	wchar_t buf [64];
	swprintf (buf, L"%lf", value);
	SetValue (buf);
}

const int TiXmlAttributeW::IntValue() const
{
	return _wtoi( value.c_str ());
}

const double  TiXmlAttributeW::DoubleValue() const
{
	return _wtof (value.c_str ());
}

void TiXmlCommentW::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ )
	{
		fputws( L"    ", cfile );
	}
	fwprintf( cfile, L"<!--%s-->", value.c_str() );
}

void TiXmlCommentW::StreamOut( TIXML_OSTREAM_W * stream ) const
{
	(*stream) << L"<!--";
	PutString( value, stream );
	(*stream) << L"-->";
}

TiXmlNodeW* TiXmlCommentW::Clone() const
{
	TiXmlCommentW* clone = new TiXmlCommentW();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


void TiXmlTextW::Print( FILE* cfile, int /*depth*/ ) const
{
	TIXML_STRING_W buffer;
	PutString( value, &buffer );
	fwprintf( cfile, L"%s", buffer.c_str() );
}


void TiXmlTextW::StreamOut( TIXML_OSTREAM_W * stream ) const
{
	PutString( value, stream );
}


TiXmlNodeW* TiXmlTextW::Clone() const
{	
	TiXmlTextW* clone = 0;
	clone = new TiXmlTextW( L"" );

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


TiXmlDeclarationW::TiXmlDeclarationW( const wchar_t * _version,
	const wchar_t * _encoding,
	const wchar_t * _standalone )
: TiXmlNodeW( TiXmlNodeW::DECLARATION )
{
	version = _version;
	encoding = _encoding;
	standalone = _standalone;
}


void TiXmlDeclarationW::Print( FILE* cfile, int /*depth*/ ) const
{
	fwprintf (cfile, L"<?xml ");

	if ( !version.empty() )
		fwprintf (cfile, L"version=\"%s\" ", version.c_str ());
	if ( !encoding.empty() )
		fwprintf (cfile, L"encoding=\"%s\" ", encoding.c_str ());
	if ( !standalone.empty() )
		fwprintf (cfile, L"standalone=\"%s\" ", standalone.c_str ());
	fwprintf (cfile, L"?>");
}

void TiXmlDeclarationW::StreamOut( TIXML_OSTREAM_W * stream ) const
{
	(*stream) << L"<?xml ";

	if ( !version.empty() )
	{
		(*stream) << L"version=\"";
		PutString( version, stream );
		(*stream) << L"\" ";
	}
	if ( !encoding.empty() )
	{
		(*stream) << L"encoding=\"";
		PutString( encoding, stream );
		(*stream ) << L"\" ";
	}
	if ( !standalone.empty() )
	{
		(*stream) << L"standalone=\"";
		PutString( standalone, stream );
		(*stream) << L"\" ";
	}
	(*stream) << L"?>";
}

TiXmlNodeW* TiXmlDeclarationW::Clone() const
{	
	TiXmlDeclarationW* clone = new TiXmlDeclarationW();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	clone->version = version;
	clone->encoding = encoding;
	clone->standalone = standalone;
	return clone;
}


void TiXmlUnknownW::Print( FILE* cfile, int depth ) const
{
	for ( int i=0; i<depth; i++ )
		fprintf( cfile, "    " );
	fwprintf( cfile, L"%s", value.c_str() );
}

void TiXmlUnknownW::StreamOut( TIXML_OSTREAM_W * stream ) const
{
	(*stream) << L"<" << value << L">";		// Don't use entities hear! It is unknown.
}

TiXmlNodeW* TiXmlUnknownW::Clone() const
{
	TiXmlUnknownW* clone = new TiXmlUnknownW();

	if ( !clone )
		return 0;

	CopyToClone( clone );
	return clone;
}


TiXmlAttributeSetW::TiXmlAttributeSetW()
{
	sentinel.next = &sentinel;
	sentinel.prev = &sentinel;
}


TiXmlAttributeSetW::~TiXmlAttributeSetW()
{
	assert( sentinel.next == &sentinel );
	assert( sentinel.prev == &sentinel );
}


void TiXmlAttributeSetW::Add( TiXmlAttributeW* addMe )
{
	assert( !Find( addMe->Name() ) );	// Shouldn't be multiply adding to the set.

	addMe->next = &sentinel;
	addMe->prev = sentinel.prev;

	sentinel.prev->next = addMe;
	sentinel.prev      = addMe;
}

void TiXmlAttributeSetW::Remove( TiXmlAttributeW* removeMe )
{
	TiXmlAttributeW* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node == removeMe )
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			node->next = 0;
			node->prev = 0;
			return;
		}
	}
	assert( 0 );		// we tried to remove a non-linked attribute.
}

TiXmlAttributeW*	TiXmlAttributeSetW::Find( const wchar_t * name ) const
{
	TiXmlAttributeW* node;

	for( node = sentinel.next; node != &sentinel; node = node->next )
	{
		if ( node->name == name )
			return node;
	}
	return 0;
}


#ifdef TIXML_USE_STL	
TIXML_ISTREAM_W & operator >> (TIXML_ISTREAM_W & in, TiXmlNodeW & base)
{
	TIXML_STRING_W tag;
	tag.reserve( 8 * 1000 );
	base.StreamIn( &in, &tag );

	base.Parse( tag.c_str() );
	return in;
}
#endif


TIXML_OSTREAM_W & operator<< (TIXML_OSTREAM_W & out, const TiXmlNodeW & base)
{
	base.StreamOut (& out);
	return out;
}










TiXmlBaseW::Entity TiXmlBaseW::entity[ NUM_ENTITY ] = 
{
	{ L"&amp;",  5, L'&' },
	{ L"&lt;",   4, L'<' },
	{ L"&gt;",   4, L'>' },
	{ L"&quot;", 6, L'\"' },
	{ L"&apos;", 6, L'\'' }
};


const wchar_t* TiXmlBaseW::SkipWhiteSpace( const wchar_t* p )
{
	if ( !p || !*p )
	{
		return 0;
	}
	while ( p && *p )
	{
		if ( *p == L' ' || *p == L'\n' || *p ==L'\r' )		// Still using old rules for white space.
			++p;
		else
			break;
	}

	return p;
}

#ifdef TIXML_USE_STL
/*static*/ bool TiXmlBaseW::StreamWhiteSpace( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag )
{
	for( ;; )
	{
		if ( !in->good() ) return false;

		int c = in->peek();
		if ( !IsWhiteSpace( c ) )
			return true;
		*tag += in->get();
	}
}

/*static*/ bool TiXmlBaseW::StreamTo( TIXML_ISTREAM_W * in, int character, TIXML_STRING_W * tag )
{
	while ( in->good() )
	{
		int c = in->peek();
		if ( c == character )
			return true;

		in->get();
		*tag += c;
	}
	return false;
}
#endif

const wchar_t* TiXmlBaseW::ReadName( const wchar_t* p, TIXML_STRING_W * name )
{
	*name = L"";
	assert( p );

	// Names start with letters or underscores.
	// After that, they can be letters, underscores, numbers,
	// hyphens, or colons. (Colons are valid ony for namespaces,
	// but tinyxml can't tell namespaces from names.)
	if (    p && *p 
		 && ( isalpha( (wchar_t) *p ) || *p == L'_' ) )
	{
		while(		p && *p
				&&	(		isalnum( (wchar_t ) *p ) 
						 || *p == L'_'
						 || *p == L'-'
						 || *p == L':' ) )
		{
			(*name) += *p;
			++p;
		}
		return p;
	}
	return 0;
}

const wchar_t* TiXmlBaseW::GetEntity( const wchar_t* p, wchar_t* value )
{
	// Presume an entity, and pull it out.
    TIXML_STRING_W ent;
	int i;

	// Ignore the &#x entities.
	if (    wcsncmp( L"&#x", p, 3 ) == 0 
	     && *(p+3) 
		 && *(p+4) )
	{
		*value = 0;
		
		if ( isalpha( *(p+3) ) ) *value += ( tolower( *(p+3) ) - L'a' + 10 ) * 16;
		else				     *value += ( *(p+3) - '0' ) * 16;

		if ( isalpha( *(p+4) ) ) *value += ( tolower( *(p+4) ) - L'a' + 10 );
		else				     *value += ( *(p+4) - '0' );

		return p+6;
	}

	// Now try to match it.
	for( i=0; i<NUM_ENTITY; ++i )
	{
		if ( wcsncmp( entity[i].str, p, entity[i].strLength ) == 0 )
		{
			assert( wcslen( entity[i].str ) == entity[i].strLength );
			*value = entity[i].chr;
			return ( p + entity[i].strLength );
		}
	}

	// So it wasn't an entity, its unrecognized, or something like that.
	*value = *p;	// Don't put back the last one, since we return it!
	return p+1;
}


bool TiXmlBaseW::StringEqual( const wchar_t* p,
							 const wchar_t* tag,
							 bool ignoreCase )
{
	assert( p );
	if ( !p || !*p )
	{
		assert( 0 );
		return false;
	}

    if ( tolower( *p ) == tolower( *tag ) )
	{
		const wchar_t* q = p;

		if (ignoreCase)
		{
			while ( *q && *tag && *q == *tag )
			{
				++q;
				++tag;
			}

			if ( *tag == 0 )		// Have we found the end of the tag, and everything equal?
			{
				return true;
			}
		}
		else
		{
			while ( *q && *tag && tolower( *q ) == tolower( *tag ) )
			{
				++q;
				++tag;
			}

			if ( *tag == 0 )
			{
				return true;
			}
		}
	}
	return false;
}

const wchar_t* TiXmlBaseW::ReadText(	const wchar_t* p, 
									TIXML_STRING_W * text, 
									bool trimWhiteSpace, 
									const wchar_t* endTag, 
									bool caseInsensitive )
{
    *text = L"";
	if (    !trimWhiteSpace			// certain tags always keep whitespace
		 || !condenseWhiteSpace )	// if true, whitespace is always kept
	{
		// Keep all the white space.
		while (	   p && *p
				&& !StringEqual( p, endTag, caseInsensitive )
			  )
		{
			wchar_t c;
			p = GetChar( p, &c );
            (* text) += c;
		}
	}
	else
	{
		bool whitespace = false;

		// Remove leading white space:
		p = SkipWhiteSpace( p );
		while (	   p && *p
				&& !StringEqual( p, endTag, caseInsensitive ) )
		{
			if ( *p == L'\r' || *p == L'\n' )
			{
				whitespace = true;
				++p;
			}
			else if ( *p == L' ')//isspace( *p ) )
			{
				whitespace = true;
				++p;
			}
			else
			{
				// If we've found whitespace, add it before the
				// new character. Any whitespace just becomes a space.
				if ( whitespace )
				{
               (* text) += L' ';
					whitespace = false;
				}
				wchar_t c;
				p = GetChar( p, &c );
            (* text) += c;
			}
		}
	}
	return p + wcslen( endTag );
}

#ifdef TIXML_USE_STL

void TiXmlDocumentW::StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag )
{
	// The basic issue with a document is that we don't know what we're
	// streaming. Read something presumed to be a tag (and hope), then
	// identify it, and call the appropriate stream method on the tag.
	//
	// This "pre-streaming" will never read the closing ">" so the
	// sub-tag can orient itself.

	if ( !StreamTo( in, L'<', tag ) ) 
	{
		SetError( TIXML_ERROR_PARSING_EMPTY );
		return;
	}

	while ( in->good() )
	{
		size_t tagIndex = tag->length();
		while ( in->good() && in->peek() != L'>' )
		{
			int c = in->get();
			(*tag) += (wchar_t) c;
		}

		if ( in->good() )
		{
			// We now have something we presume to be a node of 
			// some sort. Identify it, and call the node to
			// continue streaming.
			TiXmlNodeW* node = Identify( tag->c_str() + tagIndex );

			if ( node )
			{
				node->StreamIn( in, tag );
				bool isElement = node->ToElement() != 0;
				delete node;
				node = 0;

				// If this is the root element, we're done. Parsing will be
				// done by the >> operator.
				if ( isElement )
				{
					return;
				}
			}
			else
			{
				SetError( TIXML_ERROR );
				return;
			}
		}
	}
	// We should have returned sooner.
	SetError( TIXML_ERROR );
}

#endif

const wchar_t* TiXmlDocumentW::Parse( const wchar_t* p )
{
	// Parse away, at the document level. Since a document
	// contains nothing but other tags, most of what happens
	// here is skipping white space.
	//
	// In this variant (as opposed to stream and Parse) we
	// read everything we can.


	if ( !p || !*p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY );
		return false;
	}

    p = SkipWhiteSpace( p );
	if ( !p )
	{
		SetError( TIXML_ERROR_DOCUMENT_EMPTY );
		return false;
	}

	while ( p && *p )
	{
		TiXmlNodeW* node = Identify( p );
		if ( node )
		{
			p = node->Parse( p );
			LinkEndChild( node );
		}
		else
		{
			break;
		}
		p = SkipWhiteSpace( p );
	}
	// All is well.
	return p;
}


TiXmlNodeW* TiXmlNodeW::Identify( const wchar_t* p )
{
	TiXmlNodeW* returnNode = 0;

	p = SkipWhiteSpace( p );
	if( !p || !*p || *p != L'<' )
	{
		return 0;
	}

	TiXmlDocumentW* doc = GetDocument();
	p = SkipWhiteSpace( p );

	if ( !p || !*p )
	{
		return 0;
	}

	// What is this thing? 
	// - Elements start with a letter or underscore, but xml is reserved.
	// - Comments: <!--
	// - Decleration: <?xml
	// - Everthing else is unknown to tinyxml.
	//

	const wchar_t* xmlHeader = { L"<?xml" };
	const wchar_t* commentHeader = { L"<!--" };

	if ( StringEqual( p, xmlHeader, true ) )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Declaration\n" );
		#endif
		returnNode = new TiXmlDeclarationW();
	}
	else if (    isalpha( *(p+1) )
			  || *(p+1) == '_' )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Element\n" );
		#endif
		returnNode = new TiXmlElementW( L"" );
	}
	else if ( StringEqual( p, commentHeader, false ) )
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Comment\n" );
		#endif
		returnNode = new TiXmlCommentW();
	}
	else
	{
		#ifdef DEBUG_PARSER
			TIXML_LOG( "XML parsing Unknown\n" );
		#endif
		returnNode = new TiXmlUnknownW();
	}

	if ( returnNode )
	{
		// Set the parent, so it can report errors
		returnNode->parent = this;
		//p = returnNode->Parse( p );
	}
	else
	{
		if ( doc )
			doc->SetError( TIXML_ERROR_OUT_OF_MEMORY );
	}
	return returnNode;
}

#ifdef TIXML_USE_STL

void TiXmlElementW::StreamIn (TIXML_ISTREAM_W * in, TIXML_STRING_W * tag)
{
	// We're called with some amount of pre-parsing. That is, some of "this"
	// element is in "tag". Go ahead and stream to the closing ">"
	while( in->good() )
	{
		int c = in->get();
		(*tag) += (wchar_t) c ;
		
		if ( c == L'>' )
			break;
	}

	if ( tag->length() < 3 ) return;

	// Okay...if we are a "/>" tag, then we're done. We've read a complete tag.
	// If not, identify and stream.

	if (    tag->at( tag->length() - 1 ) == L'>' 
		 && tag->at( tag->length() - 2 ) == L'/' )
	{
		// All good!
		return;
	}
	else if ( tag->at( tag->length() - 1 ) == L'>' )
	{
		// There is more. Could be:
		//		text
		//		closing tag
		//		another node.
		for ( ;; )
		{
			StreamWhiteSpace( in, tag );

			// Do we have text?
			if ( in->peek() != L'<' )
			{
				// Yep, text.
				TiXmlTextW text( L"" );
				text.StreamIn( in, tag );

				// What follows text is a closing tag or another node.
				// Go around again and figure it out.
				continue;
			}

			// We now have either a closing tag...or another node.
			// We should be at a "<", regardless.
			if ( !in->good() ) return;
			assert( in->peek() == L'<' );
			size_t tagIndex = tag->length();

			bool closingTag = false;
			bool firstCharFound = false;

			for( ;; )
			{
				if ( !in->good() )
					return;

				int c = in->peek();
				
				if ( c == L'>' )
					break;

				*tag += c;
				in->get();

				if ( !firstCharFound && c != L'<' && !IsWhiteSpace( c ) )
				{
					firstCharFound = true;
					if ( c == L'/' )
						closingTag = true;
				}
			}
			// If it was a closing tag, then read in the closing '>' to clean up the input stream.
			// If it was not, the streaming will be done by the tag.
			if ( closingTag )
			{
				int c = in->get();
				assert( c == L'>' );
				*tag += c;

				// We are done, once we've found our closing tag.
				return;
			}
			else
			{
				// If not a closing tag, id it, and stream.
				const wchar_t* tagloc = tag->c_str() + tagIndex;
				TiXmlNodeW* node = Identify( tagloc );
				if ( !node )
					return;
				node->StreamIn( in, tag );
				delete node;
				node = 0;

				// No return: go around from the beginning: text, closing tag, or node.
			}
		}
	}
}
#endif

const wchar_t* TiXmlElementW::Parse( const wchar_t* p )
{
	p = SkipWhiteSpace( p );
	TiXmlDocumentW* document = GetDocument();

	if ( !p || !*p || *p != L'<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT );
		return false;
	}

	p = SkipWhiteSpace( p+1 );

	// Read the name.
    p = ReadName( p, &value );
	if ( !p || !*p )
	{
		if ( document )	document->SetError( TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME );
		return false;
	}

    TIXML_STRING_W endTag (L"</");
	endTag += value;
	endTag += L">";

	// Check for and read attributes. Also look for an empty
	// tag or an end tag.
	while ( p && *p )
	{
		p = SkipWhiteSpace( p );
		if ( !p || !*p )
		{
			if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
			return 0;
		}
		if ( *p == L'/' )
		{
			++p;
			// Empty tag.
			if ( *p  != L'>' )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_EMPTY );		
				return 0;
			}
			return (p+1);
		}
		else if ( *p == L'>' )
		{
			// Done with attributes (if there were any.)
			// Read the value -- which can include other
			// elements -- read the end tag, and return.
			++p;
			p = ReadValue( p );		// Note this is an Element method, and will set the error if one happens.
			if ( !p || !*p )
				return 0;

			// We should find the end tag now
			if ( StringEqual( p, endTag.c_str(), false ) )
			{
				p += endTag.length();
				return p;
			}
			else
			{
				if ( document ) document->SetError( TIXML_ERROR_READING_END_TAG );
				return 0;
			}
		}
		else
		{
			// Try to read an element:
			TiXmlAttributeW attrib;
			attrib.SetDocument( document );
			p = attrib.Parse( p );

			if ( !p || !*p )
			{
				if ( document ) document->SetError( TIXML_ERROR_PARSING_ELEMENT );
				return 0;
			}
			SetAttribute( attrib.Name(), attrib.Value() );
		}
	}
	return p;
}


const wchar_t* TiXmlElementW::ReadValue( const wchar_t* p )
{
	TiXmlDocumentW* document = GetDocument();

	// Read in text and elements in any order.
	p = SkipWhiteSpace( p );
	while ( p && *p )
	{
		if ( *p != L'<' )
		{
			// Take what we have, make a text element.
			TiXmlTextW* textNode = new TiXmlTextW( L"" );

			if ( !textNode )
			{
				if ( document ) document->SetError( TIXML_ERROR_OUT_OF_MEMORY );
				    return 0;
			}

			p = textNode->Parse( p );

			if ( !textNode->Blank() )
				LinkEndChild( textNode );
			else
				delete textNode;
		} 
		else 
		{
			// We hit a '<'
			// Have we hit a new element or an end tag?
			if ( StringEqual( p, L"</", false ) )
			{
				return p;
			}
			else
			{
				TiXmlNodeW* node = Identify( p );
				if ( node )
				{
					p = node->Parse( p );
					LinkEndChild( node );
				}				
				else
				{
					return 0;
				}
			}
		}
		p = SkipWhiteSpace( p );
	}

	if ( !p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ELEMENT_VALUE );
	}	
	return p;
}


#ifdef TIXML_USE_STL
void TiXmlUnknownW::StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag )
{
	while ( in->good() )
	{
		int c = in->get();	
		(*tag) += c;

		if ( c == L'>' )
		{
			// All is well.
			return;		
		}
	}
}
#endif


const wchar_t* TiXmlUnknownW::Parse( const wchar_t* p )
{
	TiXmlDocumentW* document = GetDocument();
	p = SkipWhiteSpace( p );
	if ( !p || !*p || *p != L'<' )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_UNKNOWN );
		return 0;
	}
	++p;
    value = L"";

	while ( p && *p && *p != L'>' )
	{
		value += *p;
		++p;
	}

	if ( !p )
	{
		if ( document )	document->SetError( TIXML_ERROR_PARSING_UNKNOWN );
	}
	if ( *p == L'>' )
		return p+1;
	return p;
}

#ifdef TIXML_USE_STL
void TiXmlCommentW::StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag )
{
	while ( in->good() )
	{
		int c = in->get();	
		(*tag) += c;

		if ( c == L'>' 
			 && tag->at( tag->length() - 2 ) == L'-'
			 && tag->at( tag->length() - 3 ) == L'-' )
		{
			// All is well.
			return;		
		}
	}
}
#endif


const wchar_t* TiXmlCommentW::Parse( const wchar_t* p )
{
	TiXmlDocumentW* document = GetDocument();
	value = L"";

	p = SkipWhiteSpace( p );
	const wchar_t* startTag = L"<!--";
	const wchar_t* endTag   = L"-->";

	if ( !StringEqual( p, startTag, false ) )
	{
		document->SetError( TIXML_ERROR_PARSING_COMMENT );
		return 0;
	}
	p += wcslen( startTag );
	p = ReadText( p, &value, false, endTag, false );
	return p;
}


const wchar_t* TiXmlAttributeW::Parse( const wchar_t* p )
{
	p = SkipWhiteSpace( p );
	if ( !p || !*p ) return 0;

	// Read the name, the '=' and the value.
	p = ReadName( p, &name );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return 0;
	}
	p = SkipWhiteSpace( p );
	if ( !p || !*p || *p != L'=' )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return 0;
	}

	++p;	// skip '='
	p = SkipWhiteSpace( p );
	if ( !p || !*p )
	{
		if ( document ) document->SetError( TIXML_ERROR_READING_ATTRIBUTES );
		return 0;
	}
	
	const wchar_t* end;

	if ( *p == L'\'' )
	{
		++p;
		end = L"\'";
		p = ReadText( p, &value, false, end, false );
	}
	else if ( *p == L'"' )
	{
		++p;
		end = L"\"";
		p = ReadText( p, &value, false, end, false );
	}
	else
	{
		// All attribute values should be in single or double quotes.
		// But this is such a common error that the parser will try
		// its best, even without them.
		value = L"";
		while (    p && *p										// existence
				&& !(*p == L' ')/*isspace( *p )*/ && *p != L'\n' && *p != L'\r'	// whitespace
				&& *p != L'/' && *p != L'>' )						// tag end
		{
			value += *p;
			++p;
		}
	}
	return p;
}

#ifdef TIXML_USE_STL
void TiXmlTextW::StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag )
{
	while ( in->good() )
	{
		int c = in->peek();	
		if ( c == L'<' )
			return;

		(*tag) += c;
		in->get();
	}
}
#endif

const wchar_t* TiXmlTextW::Parse( const wchar_t* p )
{
	value = L"";

	//TiXmlDocumentW* doc = GetDocument();
	bool ignoreWhite = true;
//	if ( doc && !doc->IgnoreWhiteSpace() ) ignoreWhite = false;

	const wchar_t* end = L"<";
	p = ReadText( p, &value, ignoreWhite, end, false );
	if ( p )
		return p-1;	// don't truncate the '<'
	return 0;
}

#ifdef TIXML_USE_STL
void TiXmlDeclarationW::StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag )
{
	while ( in->good() )
	{
		int c = in->get();
		(*tag) += c;

		if ( c == L'>' )
		{
			// All is well.
			return;
		}
	}
}
#endif

const wchar_t* TiXmlDeclarationW::Parse( const wchar_t* p )
{
	p = SkipWhiteSpace( p );
	// Find the beginning, find the end, and look for
	// the stuff in-between.
	TiXmlDocumentW* document = GetDocument();
	if ( !p || !*p || !StringEqual( p, L"<?xml", true ) )
	{
		if ( document ) document->SetError( TIXML_ERROR_PARSING_DECLARATION );
		return 0;
	}

	p += 5;
//	const char* start = p+5;
//	const char* end  = strstr( start, "?>" );

	version = L"";
	encoding = L"";
	standalone = L"";

	while ( p && *p )
	{
		if ( *p == L'>' )
		{
			++p;
			return p;
		}

		p = SkipWhiteSpace( p );
		if ( StringEqual( p, L"version", true ) )
		{
//			p += 7;
			TiXmlAttributeW attrib;
			p = attrib.Parse( p );		
			version = attrib.Value();
		}
		else if ( StringEqual( p, L"encoding", true ) )
		{
//			p += 8;
			TiXmlAttributeW attrib;
			p = attrib.Parse( p );		
			encoding = attrib.Value();
		}
		else if ( StringEqual( p, L"standalone", true ) )
		{
//			p += 10;
			TiXmlAttributeW attrib;
			p = attrib.Parse( p );		
			standalone = attrib.Value();
		}
		else
		{
			// Read over whatever it is.
			while( p && *p && *p != L'>' && !(*p == L' '))//!isspace( *p ) )
				++p;
		}
	}
	return 0;
}

bool TiXmlTextW::Blank() const
{
	for ( unsigned i=0; i<value.length(); i++ )
		if ( !(value[i] == L' '))//isspace( value[i] ) )
			return false;
	return true;
}

