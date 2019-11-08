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


#ifndef TINYXML_INCLUDEDW
#define TINYXML_INCLUDEDW

#ifdef _MSC_VER
#pragma warning( disable : 4530 )
#pragma warning( disable : 4786 )
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Help out windows:
#if defined( _DEBUG ) && !defined( DEBUG )
#define DEBUG
#endif

#if defined( DEBUG ) && defined( _MSC_VER )
#define NOMINMAX
#include <windows.h>
#define TIXML_LOG_W OutputDebugString
#else
#define TIXML_LOG_W printf
#endif

#include <string>
#include <iostream>
#define TIXML_STRING_W	std::wstring
#define TIXML_ISTREAM_W	std::wistream
#define TIXML_OSTREAM_W	std::wostream

class TiXmlDocumentW;
class TiXmlElementW;
class TiXmlCommentW;
class TiXmlUnknownW;
class TiXmlAttributeW;
class TiXmlTextW;
class TiXmlDeclarationW;


/** TiXmlBaseW is a base class for every class in TinyXml.
	It does little except to establish that TinyXml classes
	can be printed and provide some utility functions.

	In XML, the document and elements can contain
	other elements and other types of nodes.

	@verbatim
	A Document can contain:	Element	(container or leaf)
							Comment (leaf)
							Unknown (leaf)
							Declaration( leaf )

	An Element can contain:	Element (container or leaf)
							Text	(leaf)
							Attributes (not on tree)
							Comment (leaf)
							Unknown (leaf)

	A Decleration contains: Attributes (not on tree)
	@endverbatim
*/
class TiXmlBaseW
{
	friend class TiXmlNodeW;
	friend class TiXmlElementW;
	friend class TiXmlDocumentW;

public:
	TiXmlBaseW()								{}
	virtual ~TiXmlBaseW()					{}

	/**	All TinyXml classes can print themselves to a filestream.
		This is a formatted print, and will insert tabs and newlines.
		
		(For an unformatted stream, use the << operator.)
	*/
	virtual void Print( FILE* cfile, int depth ) const = 0;

	/**	The world does not agree on whether white space should be kept or
		not. In order to make everyone happy, these global, static functions
		are provided to set whether or not TinyXml will condense all white space
		into a single space or not. The default is to condense. Note changing these
		values is not thread safe.
	*/
	static void SetCondenseWhiteSpace( bool condense )		{ condenseWhiteSpace = condense; }

	/// Return the current white space setting.
	static bool IsWhiteSpaceCondensed()						{ return condenseWhiteSpace; }

protected:

	static const wchar_t*	SkipWhiteSpace( const wchar_t* );
	inline static bool	IsWhiteSpace( int c )		{ return ( isspace( c ) || c == '\n' || c == '\r' ); }

	virtual void StreamOut (TIXML_OSTREAM_W *) const = 0;

	#ifdef TIXML_USE_STL
	    static bool	StreamWhiteSpace( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	    static bool StreamTo( TIXML_ISTREAM_W * in, int character, TIXML_STRING_W * tag );
	#endif

	/*	Reads an XML name into the wstring provided. Returns
		a pointer just past the last character of the name,
		or 0 if the function has an error.
	*/
	static const wchar_t* ReadName( const wchar_t* p, TIXML_STRING_W* name );

	/*	Reads text. Returns a pointer past the given end tag.
		Wickedly complex options, but it keeps the (sensitive) code in one place.
	*/
	static const wchar_t* ReadText(	const wchar_t* in,				// where to start
									TIXML_STRING_W* text,			// the wstring read
									bool ignoreWhiteSpace,		// whether to keep the white space
									const wchar_t* endTag,			// what ends this text
									bool ignoreCase );			// whether to ignore case in the end tag
	virtual const wchar_t* Parse( const wchar_t* p ) = 0;

	// If an entity has been found, transform it into a character.
	static const wchar_t* GetEntity( const wchar_t* in, wchar_t* value );

	// Get a character, while interpreting entities.
	inline static const wchar_t* GetChar( const wchar_t* p, wchar_t* value )
	{
		assert( p );
		if ( *p == '&' )
		{
			return GetEntity( p, value );
		}
		else
		{
			*value = *p;
			return p+1;
		}
	}

	// Puts a wstring to a stream, expanding entities as it goes.
	// Note this should not contian the '<', '>', etc, or they will be transformed into entities!
	static void PutString( const TIXML_STRING_W& str, TIXML_OSTREAM_W* out );

	static void PutString( const TIXML_STRING_W& str, TIXML_STRING_W* out );

	// Return true if the next characters in the stream are any of the endTag sequences.
	bool static StringEqual(	const wchar_t* p,
								const wchar_t* endTag,
								bool ignoreCase );


	enum
	{
		TIXML_NO_ERROR = 0,
		TIXML_ERROR,
		TIXML_ERROR_OPENING_FILE,
		TIXML_ERROR_OUT_OF_MEMORY,
		TIXML_ERROR_PARSING_ELEMENT,
		TIXML_ERROR_FAILED_TO_READ_ELEMENT_NAME,
		TIXML_ERROR_READING_ELEMENT_VALUE,
		TIXML_ERROR_READING_ATTRIBUTES,
		TIXML_ERROR_PARSING_EMPTY,
		TIXML_ERROR_READING_END_TAG,
		TIXML_ERROR_PARSING_UNKNOWN,
		TIXML_ERROR_PARSING_COMMENT,
		TIXML_ERROR_PARSING_DECLARATION,
		TIXML_ERROR_DOCUMENT_EMPTY,

		TIXML_ERROR_STRING_COUNT
	};
	static const wchar_t* errorString[ TIXML_ERROR_STRING_COUNT ];

private:
	struct Entity
	{
		const wchar_t*     str;
		unsigned int	strLength;
		wchar_t		    chr;
	};
	enum
	{
		NUM_ENTITY = 5,
		MAX_ENTITY_LENGTH = 6

	};
	static Entity entity[ NUM_ENTITY ];
	static bool condenseWhiteSpace;
};


/** The parent class for everything in the Document Object Model.
	(Except for attributes, which are contained in elements.)
	Nodes have siblings, a parent, and children. A node can be
	in a document, or stand on its own. The type of a TiXmlNodeW
	can be queried, and it can be cast to its more defined type.
*/
class TiXmlNodeW : public TiXmlBaseW
{
	friend class TiXmlDocumentW;
	friend class TiXmlElementW;

public:
	#ifdef TIXML_USE_STL	

	    /** An input stream operator, for every class. Tolerant of newlines and
		    formatting, but doesn't expect them.
	    */
	    friend std::istream& operator >> (std::istream& in, TiXmlNodeW& base);

	    /** An output stream operator, for every class. Note that this outputs
		    without any newlines or formatting, as opposed to Print(), which
		    includes tabs and new lines.

		    The operator<< and operator>> are not completely symmetric. Writing
		    a node to a stream is very well defined. You'll get a nice stream
		    of output, without any extra whitespace or newlines.
		    
		    But reading is not as well defined. (As it always is.) If you create
		    a TiXmlElementW (for example) and read that from an input stream,
		    the text needs to define an element or junk will result. This is
		    true of all input streams, but it's worth keeping in mind.

		    A TiXmlDocumentW will read nodes until it reads a root element.
	    */	
	    friend std::ostream & operator<< (std::ostream& out, const TiXmlNodeW& base);

	#else
	    // Used internally, not part of the public API.
	    friend TIXML_OSTREAM_W& operator<< (TIXML_OSTREAM_W& out, const TiXmlNodeW& base);
	#endif

	/** The types of XML nodes supported by TinyXml. (All the
			unsupported types are picked up by UNKNOWN.)
	*/
	enum NodeType
	{
		DOCUMENT,
		ELEMENT,
		COMMENT,
		UNKNOWN,
		TEXT,
		DECLARATION,
		TYPECOUNT
	};

	virtual ~TiXmlNodeW();

	/** The meaning of 'value' changes for the specific type of
		TiXmlNodeW.
		@verbatim
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text wstring
		@endverbatim

		The subclasses will wrap this function.
	*/
	const wchar_t * Value () const { return value.c_str (); }

	/** Changes the value of the node. Defined as:
		@verbatim
		Document:	filename of the xml file
		Element:	name of the element
		Comment:	the comment text
		Unknown:	the tag contents
		Text:		the text wstring
		@endverbatim
	*/
	void SetValue (const wchar_t * _value) { value = _value;}

    #ifdef TIXML_USE_STL
	/// STL std::wstring form.
	void SetValue( const std::wstring& value )    
	{	  
		SetValue( value.c_str() );    	
	}	
	#endif

	/// Delete all the children of this node. Does not affect 'this'.
	void Clear();

	/// One step up the DOM.
	TiXmlNodeW* Parent() const					{ return parent; }

	TiXmlNodeW* FirstChild()	const	{ return firstChild; }		///< The first child of this node. Will be null if there are no children.
	TiXmlNodeW* FirstChild( const wchar_t * value ) const;			///< The first child of this node with the matching 'value'. Will be null if none found.

	TiXmlNodeW* LastChild() const	{ return lastChild; }		/// The last child of this node. Will be null if there are no children.
	TiXmlNodeW* LastChild( const wchar_t * value ) const;			/// The last child of this node matching 'value'. Will be null if there are no children.

    #ifdef TIXML_USE_STL
	TiXmlNodeW* FirstChild( const std::wstring& value ) const	{	return FirstChild (value.c_str ());	}	///< STL std::wstring form.
	TiXmlNodeW* LastChild( const std::wstring& value ) const	{	return LastChild (value.c_str ());	}	///< STL std::wstring form.
	#endif

	/** An alternate way to walk the children of a node.
		One way to iterate over nodes is:
		@verbatim
			for( child = parent->FirstChild(); child; child = child->NextSibling() )
		@endverbatim

		IterateChildren does the same thing with the syntax:
		@verbatim
			child = 0;
			while( child = parent->IterateChildren( child ) )
		@endverbatim

		IterateChildren takes the previous child as input and finds
		the next one. If the previous child is null, it returns the
		first. IterateChildren will return null when done.
	*/
	TiXmlNodeW* IterateChildren( TiXmlNodeW* previous ) const;

	/// This flavor of IterateChildren searches for children with a particular 'value'
	TiXmlNodeW* IterateChildren( const wchar_t * value, TiXmlNodeW* previous ) const;

    #ifdef TIXML_USE_STL
	TiXmlNodeW* IterateChildren( const std::wstring& value, TiXmlNodeW* previous ) const	{	return IterateChildren (value.c_str (), previous);	}	///< STL std::wstring form.
	#endif

	/** Add a new node related to this. Adds a child past the LastChild.
		Returns a pointer to the new object or NULL if an error occured.
	*/
	TiXmlNodeW* InsertEndChild( const TiXmlNodeW& addThis );

	/** Add a new node related to this. Adds a child before the specified child.
		Returns a pointer to the new object or NULL if an error occured.
	*/
	TiXmlNodeW* InsertBeforeChild( TiXmlNodeW* beforeThis, const TiXmlNodeW& addThis );

	/** Add a new node related to this. Adds a child after the specified child.
		Returns a pointer to the new object or NULL if an error occured.
	*/
	TiXmlNodeW* InsertAfterChild(  TiXmlNodeW* afterThis, const TiXmlNodeW& addThis );

	/** Replace a child of this node.
		Returns a pointer to the new object or NULL if an error occured.
	*/
	TiXmlNodeW* ReplaceChild( TiXmlNodeW* replaceThis, const TiXmlNodeW& withThis );

	/// Delete a child of this node.
	bool RemoveChild( TiXmlNodeW* removeThis );

	/// Navigate to a sibling node.
	TiXmlNodeW* PreviousSibling() const			{ return prev; }

	/// Navigate to a sibling node.
	TiXmlNodeW* PreviousSibling( const wchar_t * ) const;

    #ifdef TIXML_USE_STL
	TiXmlNodeW* PreviousSibling( const std::wstring& value ) const	{	return PreviousSibling (value.c_str ());	}	///< STL std::wstring form.
	TiXmlNodeW* NextSibling( const std::wstring& value) const	{	return NextSibling (value.c_str ());	}	///< STL std::wstring form.
	#endif

	/// Navigate to a sibling node.
	TiXmlNodeW* NextSibling() const				{ return next; }

	/// Navigate to a sibling node with the given 'value'.
	TiXmlNodeW* NextSibling( const wchar_t * ) const;

	/** Convenience function to get through elements.
		Calls NextSibling and ToElement. Will skip all non-Element
		nodes. Returns 0 if there is not another element.
	*/
	TiXmlElementW* NextSiblingElement() const;

	/** Convenience function to get through elements.
		Calls NextSibling and ToElement. Will skip all non-Element
		nodes. Returns 0 if there is not another element.
	*/
	TiXmlElementW* NextSiblingElement( const wchar_t * ) const;

    #ifdef TIXML_USE_STL
	TiXmlElementW* NextSiblingElement( const std::wstring& value) const	{	return NextSiblingElement (value.c_str ());	}	///< STL std::wstring form.
	#endif

	/// Convenience function to get through elements.
	TiXmlElementW* FirstChildElement()	const;

	/// Convenience function to get through elements.
	TiXmlElementW* FirstChildElement( const wchar_t * value ) const;

    #ifdef TIXML_USE_STL
	TiXmlElementW* FirstChildElement( const std::wstring& value ) const	{	return FirstChildElement (value.c_str ());	}	///< STL std::wstring form.
	#endif

	/// Query the type (as an enumerated value, above) of this node.
	virtual int Type() const	{ return type; }

	/** Return a pointer to the Document this node lives in.
		Returns null if not in a document.
	*/
	TiXmlDocumentW* GetDocument() const;

	/// Returns true if this node has no children.
	bool NoChildren() const						{ return !firstChild; }

	TiXmlDocumentW* ToDocument()	const		{ return ( this && type == DOCUMENT ) ? (TiXmlDocumentW*) this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlElementW*  ToElement() const		{ return ( this && type == ELEMENT  ) ? (TiXmlElementW*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlCommentW*  ToComment() const		{ return ( this && type == COMMENT  ) ? (TiXmlCommentW*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlUnknownW*  ToUnknown() const		{ return ( this && type == UNKNOWN  ) ? (TiXmlUnknownW*)  this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlTextW*	   ToText()    const		{ return ( this && type == TEXT     ) ? (TiXmlTextW*)     this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.
	TiXmlDeclarationW* ToDeclaration() const	{ return ( this && type == DECLARATION ) ? (TiXmlDeclarationW*) this : 0; } ///< Cast to a more defined type. Will return null not of the requested type.

	virtual TiXmlNodeW* Clone() const = 0;

	void  SetUserData( void* user )			{ userData = user; }
	void* GetUserData()						{ return userData; }

protected:
	TiXmlNodeW( NodeType type );

	#ifdef TIXML_USE_STL
	    // The real work of the input operator.
	    virtual void StreamIn( TIXML_ISTREAM_W* in, TIXML_STRING_W* tag ) = 0;
	#endif

	// The node is passed in by ownership. This object will delete it.
	TiXmlNodeW* LinkEndChild( TiXmlNodeW* addThis );

	// Figure out what is at *p, and parse it. Returns null if it is not an xml node.
	TiXmlNodeW* Identify( const wchar_t* start );
	void CopyToClone( TiXmlNodeW* target ) const	{ target->SetValue (value.c_str() );
												  target->userData = userData; }

	// Internal Value function returning a TIXML_STRING_W
	TIXML_STRING_W SValue() const	{ return value ; }

	TiXmlNodeW*		parent;
	NodeType		type;

	TiXmlNodeW*		firstChild;
	TiXmlNodeW*		lastChild;

	TIXML_STRING_W	value;

	TiXmlNodeW*		prev;
	TiXmlNodeW*		next;
	void*			userData;
};


/** An attribute is a name-value pair. Elements have an arbitrary
	number of attributes, each with a unique name.

	@note The attributes are not TiXmlNodes, since they are not
		  part of the tinyXML document object model. There are other
		  suggested ways to look at this problem.

	@note Attributes have a parent
*/
class TiXmlAttributeW : public TiXmlBaseW
{
	friend class TiXmlAttributeSetW;

public:
	/// Construct an empty attribute.
	TiXmlAttributeW() : prev( 0 ), next( 0 )	{}

	#ifdef TIXML_USE_STL
	/// std::wstring constructor.
	TiXmlAttributeW( const std::wstring& _name, const std::wstring& _value )
	{
		name = _name;
		value = _value;
	}
	#endif

	/// Construct an attribute with a name and value.
	TiXmlAttributeW( const wchar_t * _name, const wchar_t * _value ): name( _name ), value( _value ), prev( 0 ), next( 0 ) {}
	const wchar_t*		Name()  const		{ return name.c_str (); }		///< Return the name of this attribute.
	const wchar_t*		Value() const		{ return value.c_str (); }		///< Return the value of this attribute.
	const int       IntValue() const;									///< Return the value of this attribute, converted to an integer.
	const double	DoubleValue() const;								///< Return the value of this attribute, converted to a double.

	void SetName( const wchar_t* _name )	{ name = _name; }				///< Set the name of this attribute.
	void SetValue( const wchar_t* _value )	{ value = _value; }				///< Set the value.

	void SetIntValue( int value );										///< Set the value from an integer.
	void SetDoubleValue( double value );								///< Set the value from a double.

    #ifdef TIXML_USE_STL
	/// STL std::wstring form.
	void SetName( const std::wstring& _name )	
	{	
		SetName(_name.c_str() );	
	}
	/// STL std::wstring form.	
	void SetValue( const std::wstring& _value )	
	{	
		SetValue( value.c_str() );	
	}
	#endif

	/// Get the next sibling attribute in the DOM. Returns null at end.
	TiXmlAttributeW* Next() const;
	/// Get the previous sibling attribute in the DOM. Returns null at beginning.
	TiXmlAttributeW* Previous() const;

	bool operator==( const TiXmlAttributeW& rhs ) const { return rhs.name == name; }
	bool operator<( const TiXmlAttributeW& rhs )	 const { return name < rhs.name; }
	bool operator>( const TiXmlAttributeW& rhs )  const { return name > rhs.name; }

	/*	[internal use]
		Attribtue parsing starts: first letter of the name
						 returns: the next wchar_t after the value end quote
	*/
	virtual const wchar_t* Parse( const wchar_t* p );

	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;

	virtual void StreamOut( TIXML_OSTREAM_W * out ) const;
	// [internal use]
	// Set the document pointer so the attribute can report errors.
	void SetDocument( TiXmlDocumentW* doc )	{ document = doc; }

private:
	TiXmlDocumentW*	document;	// A pointer back to a document, for error reporting.
	TIXML_STRING_W name;
	TIXML_STRING_W value;
	TiXmlAttributeW*	prev;
	TiXmlAttributeW*	next;
};


/*	A class used to manage a group of attributes.
	It is only used internally, both by the ELEMENT and the DECLARATION.
	
	The set can be changed transparent to the Element and Declaration
	classes that use it, but NOT transparent to the Attribute
	which has to implement a next() and previous() method. Which makes
	it a bit problematic and prevents the use of STL.

	This version is implemented with circular lists because:
		- I like circular lists
		- it demonstrates some independence from the (typical) doubly linked list.
*/
class TiXmlAttributeSetW
{
public:
	TiXmlAttributeSetW();
	~TiXmlAttributeSetW();

	void Add( TiXmlAttributeW* attribute );
	void Remove( TiXmlAttributeW* attribute );

	TiXmlAttributeW* First() const	{ return ( sentinel.next == &sentinel ) ? 0 : sentinel.next; }
	TiXmlAttributeW* Last()  const	{ return ( sentinel.prev == &sentinel ) ? 0 : sentinel.prev; }
	TiXmlAttributeW*	Find( const wchar_t * name ) const;

private:
	TiXmlAttributeW sentinel;
};


/** The element is a container class. It has a value, the element name,
	and can contain other elements, text, comments, and unknowns.
	Elements also contain an arbitrary number of attributes.
*/
class TiXmlElementW : public TiXmlNodeW
{
public:
	/// Construct an element.
	TiXmlElementW (const wchar_t * in_value);

	#ifdef TIXML_USE_STL
	/// std::wstring constructor.
	TiXmlElementW( const std::wstring& _value ) : 	TiXmlNodeW( TiXmlNodeW::ELEMENT )
	{
		firstChild = lastChild = 0;
		value = _value;
	}
	#endif

	virtual ~TiXmlElementW();

// OGRE CHANGE 
// Resolve ambiguity of std::wstring / const wchar_t* conversion of Ogre::String
#ifndef TIXML_USE_STL
// OGRE CHANGE
	/** Given an attribute name, attribute returns the value
		for the attribute of that name, or null if none exists.
	*/
	const wchar_t* Attribute( const wchar_t* name ) const;

	/** Given an attribute name, attribute returns the value
		for the attribute of that name, or null if none exists.
		If the attribute exists and can be converted to an integer,
		the integer value will be put in the return 'i', if 'i'
		is non-null.
	*/
	const wchar_t* Attribute( const wchar_t* name, int* i ) const;

	/** 获取一个浮点数
	*/
	const wchar_t* Attribute( const wchar_t* name, float* f) const;

	/** Sets an attribute of name to a given value. The attribute
		will be created if it does not exist, or changed if it does.
	*/
	void SetAttribute( const wchar_t* name, const wchar_t * value );
// OGRE CHANGE
#endif
// OGRE CHANGE
    #ifdef TIXML_USE_STL
	const wchar_t* Attribute( const std::wstring& name ) const;
	const wchar_t* Attribute( const std::wstring& name, int* i ) const;

	/// STL std::wstring form.
	void SetAttribute( const std::wstring& name, const std::wstring& value );
	///< STL std::wstring form.
	void SetAttribute( const std::wstring& name, int value );
	#endif

	/** Sets an attribute of name to a given value. The attribute
		will be created if it does not exist, or changed if it does.
	*/
	void SetAttribute( const wchar_t * name, int value );

	/** Deletes an attribute with the given name.
	*/
	void RemoveAttribute( const wchar_t * name );
    #ifdef TIXML_USE_STL
	void RemoveAttribute( const std::wstring& name )	{	RemoveAttribute (name.c_str ());	}	///< STL std::wstring form.
	#endif

	TiXmlAttributeW* FirstAttribute() const	{ return attributeSet.First(); }		///< Access the first attribute in this element.
	TiXmlAttributeW* LastAttribute()	const 	{ return attributeSet.Last(); }		///< Access the last attribute in this element.

	// [internal use] Creates a new Element and returs it.
	virtual TiXmlNodeW* Clone() const;
	// [internal use]

	virtual void Print( FILE* cfile, int depth ) const;
	const wchar_t* GetValue(){return this->value.c_str();}

protected:

	// Used to be public [internal use]
	#ifdef TIXML_USE_STL
	    virtual void StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	#endif
	virtual void StreamOut( TIXML_OSTREAM_W * out ) const;

	/*	[internal use]
		Attribtue parsing starts: next wchar_t past '<'
						 returns: next wchar_t past '>'
	*/
	virtual const wchar_t* Parse( const wchar_t* p );

	/*	[internal use]
		Reads the "value" of the element -- another element, or text.
		This should terminate with the current end tag.
	*/
	const wchar_t* ReadValue( const wchar_t* in );

private:
	TiXmlAttributeSetW attributeSet;
};


/**	An XML comment.
*/
class TiXmlCommentW : public TiXmlNodeW
{
public:
	/// Constructs an empty comment.
	TiXmlCommentW() : TiXmlNodeW( TiXmlNodeW::COMMENT ) {}
	virtual ~TiXmlCommentW()	{}

	// [internal use] Creates a new Element and returs it.
	virtual TiXmlNodeW* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;
protected:
	// used to be public
	#ifdef TIXML_USE_STL
	    virtual void StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	#endif
	virtual void StreamOut( TIXML_OSTREAM_W * out ) const;
	/*	[internal use]
		Attribtue parsing starts: at the ! of the !--
						 returns: next wchar_t past '>'
	*/
	virtual const wchar_t* Parse( const wchar_t* p );
};


/** XML text. Contained in an element.
*/
class TiXmlTextW : public TiXmlNodeW
{
	friend class TiXmlElementW;
public:
	/// Constructor.
	TiXmlTextW (const wchar_t * initValue) : TiXmlNodeW (TiXmlNodeW::TEXT)
	{
		SetValue( initValue );
	}
	virtual ~TiXmlTextW() {}

	#ifdef TIXML_USE_STL
	/// Constructor.
	TiXmlTextW( const std::wstring& initValue ) : TiXmlNodeW (TiXmlNodeW::TEXT)
	{
		SetValue( initValue );
	}
	#endif

protected :
	// [internal use] Creates a new Element and returns it.
	virtual TiXmlNodeW* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;
	virtual void StreamOut ( TIXML_OSTREAM_W * out ) const;
	// [internal use]
	bool Blank() const;	// returns true if all white space and new lines
	/*	[internal use]
			Attribtue parsing starts: First wchar_t of the text
							 returns: next wchar_t past '>'
		*/
	virtual const wchar_t* Parse( const wchar_t* p );
	// [internal use]
	#ifdef TIXML_USE_STL
	    virtual void StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	#endif
};


/** In correct XML the declaration is the first entry in the file.
	@verbatim
		<?xml version="1.0" standalone="yes"?>
	@endverbatim

	TinyXml will happily read or write files without a declaration,
	however. There are 3 possible attributes to the declaration:
	version, encoding, and standalone.

	Note: In this version of the code, the attributes are
	handled as special cases, not generic attributes, simply
	because there can only be at most 3 and they are always the same.
*/
class TiXmlDeclarationW : public TiXmlNodeW
{
public:
	/// Construct an empty declaration.
	TiXmlDeclarationW()   : TiXmlNodeW( TiXmlNodeW::DECLARATION ) {}

#ifdef TIXML_USE_STL
	/// Constructor.
	TiXmlDeclarationW(
						const std::wstring& _version,
						const std::wstring& _encoding,
						const std::wstring& _standalone )
					: TiXmlNodeW( TiXmlNodeW::DECLARATION )
	{
		version = _version;
		encoding = _encoding;
		standalone = _standalone;
	}
#endif

	/// Construct.
	TiXmlDeclarationW( const wchar_t * _version,
										const wchar_t * _encoding,
										const wchar_t * _standalone );

	virtual ~TiXmlDeclarationW()	{}

	/// Version. Will return empty if none was found.
	const wchar_t * Version() const		{ return version.c_str (); }
	/// Encoding. Will return empty if none was found.
	const wchar_t * Encoding() const		{ return encoding.c_str (); }
	/// Is this a standalone document?
	const wchar_t * Standalone() const		{ return standalone.c_str (); }

	// [internal use] Creates a new Element and returs it.
	virtual TiXmlNodeW* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;

protected:
	// used to be public
	#ifdef TIXML_USE_STL
	    virtual void StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	#endif
	virtual void StreamOut ( TIXML_OSTREAM_W * out) const;
	//	[internal use]
	//	Attribtue parsing starts: next wchar_t past '<'
	//					 returns: next wchar_t past '>'

	virtual const wchar_t* Parse( const wchar_t* p );

private:
	TIXML_STRING_W version;
	TIXML_STRING_W encoding;
	TIXML_STRING_W standalone;
};


/** Any tag that tinyXml doesn't recognize is save as an
	unknown. It is a tag of text, but should not be modified.
	It will be written back to the XML, unchanged, when the file
	is saved.
*/
class TiXmlUnknownW : public TiXmlNodeW
{
public:
	TiXmlUnknownW() : TiXmlNodeW( TiXmlNodeW::UNKNOWN ) {}
	virtual ~TiXmlUnknownW() {}

	// [internal use]
	virtual TiXmlNodeW* Clone() const;
	// [internal use]
	virtual void Print( FILE* cfile, int depth ) const;
protected:
	// used to be public
	#ifdef TIXML_USE_STL
	    virtual void StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	#endif
	virtual void StreamOut ( TIXML_OSTREAM_W * out ) const;
	/*	[internal use]
		Attribute parsing starts: First wchar_t of the text
						 returns: next wchar_t past '>'
	*/
	virtual const wchar_t* Parse( const wchar_t* p );
};


/** Always the top level node. A document binds together all the
	XML pieces. It can be saved, loaded, and printed to the screen.
	The 'value' of a document node is the xml file name.
*/
class TiXmlDocumentW : public TiXmlNodeW
{
public:
	/// Create an empty document, that has no name.
	TiXmlDocumentW();
	/// Create a document with a name. The name of the document is also the filename of the xml.
	TiXmlDocumentW( const wchar_t * documentName );

	#ifdef TIXML_USE_STL
	/// Constructor.
	TiXmlDocumentW( const std::wstring& documentName ) :
	    TiXmlNodeW( TiXmlNodeW::DOCUMENT )
	{
        value = documentName;
		error = false;
	}
	#endif

	virtual ~TiXmlDocumentW() {}

	/** Load a file using the current document value.
		Returns true if successful. Will delete any existing
		document data before loading.
	*/
	bool LoadFromMemory(const wchar_t* szText);
	//
	bool LoadFile();
	/// Save a file using the current document value. Returns true if successful.
	bool SaveFile( const wchar_t* szHeadText ) const;
	/// Load a file using the given filename. Returns true if successful.
	bool LoadFile( const wchar_t * filename );
	/// Save a file using the given filename. Returns true if successful.
	bool SaveFile( const wchar_t * filename , const wchar_t* szHeadText ) const;

	#ifdef TIXML_USE_STL
	bool LoadFile( const std::wstring& filename )			///< STL std::wstring version.
	{
		return ( LoadFile( filename.c_str() ));
	}
	bool SaveFile( const std::wstring& filename  , const wchar_t* szHeadText ) const		///< STL std::wstring version.
	{
		//StringToBuffer f( filename );
		return ( SaveFile( filename.c_str() , szHeadText));
	}
	#endif

	/// Parse the given null terminated block of xml data.
	virtual const wchar_t* Parse( const wchar_t* p );

	/** Get the root element -- the only top level element -- of the document.
		In well formed XML, there should only be one. TinyXml is tolerant of
		multiple elements at the document level.
	*/
	TiXmlElementW* RootElement() const		{ return FirstChildElement(); }

	/// If, during parsing, a error occurs, Error will be set to true.
	bool Error() const						{ return error; }

	/// Contains a textual (english) description of the error if one occurs.
	const wchar_t * ErrorDesc() const	{ return errorDesc.c_str (); }

	/** Generally, you probably want the error wstring ( ErrorDesc() ). But if you
			prefer the ErrorId, this function will fetch it.
		*/
	const int ErrorId()	const				{ return errorId; }

	/// If you have handled the error, it can be reset with this call.
	void ClearError()						{ error = false; errorId = 0; errorDesc = L""; }

	/** Dump the document to standard out. */
	void Print() const						{ Print( stdout, 0 ); }

	// [internal use]
	virtual void Print( FILE* cfile, int depth = 0 ) const;
	// [internal use]
	void SetError( int err ) {		assert( err > 0 && err < TIXML_ERROR_STRING_COUNT );
		error   = true;
		errorId = err;
	errorDesc = errorString[ errorId ]; }

protected :
	virtual void StreamOut ( TIXML_OSTREAM_W * out) const;
	// [internal use]
	virtual TiXmlNodeW* Clone() const;
	#ifdef TIXML_USE_STL
	    virtual void StreamIn( TIXML_ISTREAM_W * in, TIXML_STRING_W * tag );
	#endif

private:
	bool error;
	int  errorId;
	TIXML_STRING_W errorDesc;
};

#endif

