/** 
 * Copyright (C) 2001 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef KMAYBE_H
#define KMAYBE_H

#include <assert.h>
#include <iostream>

/**
 * Maybe template class. It is used instead of a pointer to model variables
 * which can have either no or exactly one value assigned to them.
 */
template< class Type >
class KMaybe
{
public:
    /**
     * Constructs an empty container.
     */
    KMaybe() 
    {
	_dataPtr = 0;
    }

    /**
     * Constructs a container containing a copy of t.
     */
    KMaybe( const Type& t ) 
    {
	_dataPtr = new Type( t );
    }

    /**
     * Constructs a deep copy of m.
     */
    KMaybe( const KMaybe< Type >& m ) 
    {
	if( m.isNull() )
	    _dataPtr = 0;
	else
	    _dataPtr = new Type( m.data() );
    }

    ~KMaybe() 
    {
	setNull();
    }

    KMaybe< Type >& operator = ( const Type& t ) 
    {
	if( isNull() )
	    _dataPtr = new Type( t );
	else
	    *_dataPtr = t;
	return (*this);
    }

    KMaybe< Type >& operator = ( const KMaybe< Type >& m ) 
    {
	if( *this != m )
	{
	    if( m.isNull() )
		setNull();
	    else
		operator = ( m.data() );
	}
	return (*this);
    }

    bool operator == ( const Type& t ) const 
    {
	if( isNull() ) 
	    return false;
	else 
	    return ( data() == t );
    }

    bool operator != ( const Type& t ) const 
    {
	if( isNull() ) 
	    return true;
	else 
	    return ( data() != t );
    }

    bool operator == ( const KMaybe< Type >& t ) const 
    {
	if( isNull() && t.isNull() ) 
	    return true;
	else if( isNull() || t.isNull() ) 
	    return false;
	else 
	    return ( data() == t.data() );
    }

    bool operator != ( const KMaybe< Type >& t ) const 
    {
	if( isNull() && t.isNull() ) 
	    return false;
	else if( isNull() || t.isNull() ) 
	    return true;
	else 
	    return ( data() != t.data() );
    }

    /**
     * Returns true if the container doesn't contain a value, false otherwise.
     */
    bool operator ! () const 
    {
	return isNull();
    }

    /**
     * Returns the contents of the container. You should check first if
     * the container isn't empty with isNull().
     */
    Type data() const 
    {
	assert( _dataPtr != 0 ); return *_dataPtr;
    }

    /**
     * Operator which can be used instead of data().
     */
    Type& operator * () 
    {
	return data();
    }
    const Type& operator * () const 
    {
	return data();
    }

    Type* operator -> () 
    {
	assert( _dataPtr != 0 ); return _dataPtr;
    }
    const Type* operator -> () const 
    {
	assert( _dataPtr != 0 ); return _dataPtr;
    }

    /**
     * Returns true if the container doesn't contain a value, false otherwise.
     */
    bool isNull() const 
    {
	return _dataPtr == 0;
    }

    /**
     * Reset the contents of the container to Null.
     */
    void setNull() 
    {
	if( _dataPtr ) {
	    delete _dataPtr;
	    _dataPtr = 0;
	}
    }

private:
    Type* _dataPtr;
};


template< class Type >
std::ostream& operator << ( std::ostream& os, const KMaybe< Type >& source )
{
    if( source.isNull() )
	os << "Null";
    else
	os << source.data();
    return os;
}

template< class Type >
std::istream& operator >> ( std::istream& is, KMaybe< Type >& destination )
{
    Type t;
    is >> t;
    destination = t;
    return is;
}

#endif
