/* This file is part of the KDE project
 * Copyright (C) 2002 Matthias Witzgall <witzi@gmx.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */


/* further informations about the dvi file format could be downloaded from: http://www.rpi.edu/~sofkam/DVI/archive/standards/dvistd0.dvi */

#include "kfile_dvi.h"

#include <kgenericfactory.h>
#include <kdebug.h>
#include <klocale.h>
#include <kfilemetainfo.h>

#include <qstring.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qregexp.h>


// preprocessormacro K_EXPORT_COMPONENT_FACTORY loads shared library 'kfile_dvi.so' dynamic if necessary
typedef KGenericFactory<KDviPlugin> DviFactory;
K_EXPORT_COMPONENT_FACTORY(kfile_dvi, DviFactory("kfile_dvi"));

KDviPlugin::KDviPlugin (QObject * parent, const char * name, const QStringList & preferredItems)
	: KFilePlugin(parent, name, preferredItems)
{
	kdDebug(7034) << "dvi plugin\n";

	// set up our mime type
	KFileMimeTypeInfo * info = this->addMimeTypeInfo("application/x-dvi");


	KFileMimeTypeInfo::GroupInfo * group = this->addGroupInfo(info, "General", "General");

	this->addItemInfo(group, "1_Type", i18n("Type"), QVariant::String);  // numbers are added to display the keys in the right order
	this->addItemInfo(group, "2_Size", i18n("Size"), QVariant::String);
	this->addItemInfo(group, "3_Created", i18n("Created"), QVariant::String);
	this->addItemInfo(group, "4_Modified", i18n("Modified"), QVariant::String);
	this->addItemInfo(group, "5_Permissions", i18n("Permissions"), QVariant::String);
	this->addItemInfo(group, "6_Comment", i18n("Comment"), QVariant::String);
	this->addItemInfo(group, "7_Pages", i18n("Pages"), QVariant::UInt);
}

bool KDviPlugin::readInfo (KFileMetaInfo & info, uint /* what (unused in this plugin) */)
{
	KFileMetaInfoGroup GeneralGroup = appendGroup(info, "General");
	QFile f(info.path());
	QFileInfo f_info;
	Q_UINT16 bytes_to_read;
	Q_UINT8 comment_length;
	QString comment;
	QRegExp pattern(" TeX output \\d\\d\\d\\d\\.\\d\\d\\.\\d\\d:\\d\\d\\d\\d"); // a regular expression for the known format of comment
	QString creation_date;
	Q_UINT16 pages;
	Q_UINT8 buffer[270]; // buffer for reading data; no data is read with more than 270 bytes
	QString permissions_string = "-"; // it is a file in every case
	double size;
	QString size_string;
	Q_UINT32 ptr;
	int i; // running index




// open file and try to get the comment
	f.open(IO_ReadOnly);

	if ( f.isOpen() == false ){
		kdDebug(7034) << "cannot open file\n";
		return false;
	}

	f_info.setFile(f); // create fileinfoobject
	bytes_to_read = QMIN(f_info.size(), 270); // check, if the file's size is smaller than 270 bytes
						  // (if the comment is as large as possible, we don't have to
						  // read more than 270 bytes)

	if ( f.readBlock((char *)buffer, bytes_to_read) != bytes_to_read ){ // cast to (char *) is necessary
		kdDebug(7034) << "read error (1)\n";
		return false;
	}

	if ( (buffer[0] != 247)  ||  (buffer[1] != 2) ){
		// magic numbers are not right
		kdDebug(7034) << "wrong file format\n";
		return false;
	}

	comment_length = buffer[14]; // set up length of comment
	comment.setLength(comment_length); // used to avoid permanent reallocation when extracting the comment from buffer

	for ( i = 15; i <= 14+comment_length; ++i ){ // extract comment from buffer
		comment[i-15] = (char)buffer[i];
	}

	if ( pattern.exactMatch(comment) == true ){ // check, if format of comment is known

		kdDebug(7034) << "known format of comment; display Created instead of Comment\n";
		creation_date = comment.right(15);
		creation_date[4] = '-';
		creation_date[7] = '-';
		creation_date[10] = ' ';
		creation_date.insert(13, ':');
		appendItem(GeneralGroup, "3_Created", QVariant(creation_date) ); // ISO 8601 date format (without seconds)

	} else {

		appendItem(GeneralGroup, "6_Comment", QVariant(comment) );

	}


// comment is ok, now get total number of pages
	f.at( f.size() - 13);
	if ( f.readBlock((char *)buffer, 13) != 13 ){
		kdDebug(7034) << "read error (2)\n";
		return false;
	}

	i = 12; // reset running index i
	while ( buffer[i] == 223 ){ --i; } // skip all trailing bytes

	if ( (buffer[i] != 2) || (i > 8) || (i < 5) ){
		kdDebug(7034) << "wrong file format\n";
		return false;
	}

	// now we know the position of the pointer to the beginning of the postamble and we can read it
	ptr = buffer[i-4];
	ptr = (ptr << 8) | buffer[i-3];
	ptr = (ptr << 8) | buffer[i-2];
	ptr = (ptr << 8) | buffer[i-1];

	// bytes for total number of pages have a offset of 27 to the beginning of the postamble
	f.at(ptr + 27);

	// now read total number of pages from file
	if ( f.readBlock((char *)buffer, 2) != 2 ){
		kdDebug(7034) << "read error (3)\n";
		return false;
	}
	pages = buffer[0];
	pages = (pages << 8) | buffer[1];

	appendItem(GeneralGroup, "7_Pages", QVariant(pages) );

	f.close();

// now get and set up some basic informations about the file (same informations would be displayed, if there is no dvi-plugin)
	appendItem(GeneralGroup, "1_Type", QVariant( i18n("TeX Device Independent file") ) ); // set up type of file

	appendItem(GeneralGroup, "4_Modified", QVariant(f_info.lastModified().toString("yyyy-MM-dd hh:mm")) );
											// ISO 8601 date format (without seconds)


// set up permissions string

	// User
	if ( f_info.permission(QFileInfo::ReadUser) == true ){
		permissions_string.append('r');
	} else {
		permissions_string.append('-');
	}

	if ( f_info.permission(QFileInfo::WriteUser) == true ){
		permissions_string.append('w');
	} else {
		permissions_string.append('-');
	}

	if ( f_info.permission(QFileInfo::ExeUser) == true ){
		permissions_string.append('x');
	} else {
		permissions_string.append('-');
	}

	// Group
	if ( f_info.permission(QFileInfo::ReadGroup) == true ){
		permissions_string.append('r');
	} else {
		permissions_string.append('-');
	}

	if ( f_info.permission(QFileInfo::WriteGroup) == true ){
		permissions_string.append('w');
	} else {
		permissions_string.append('-');
	}

	if ( f_info.permission(QFileInfo::ExeGroup) == true ){
		permissions_string.append('x');
	} else {
		permissions_string.append('-');
	}

	// Other
	if ( f_info.permission(QFileInfo::ReadOther) == true ){
		permissions_string.append('r');
	} else {
		permissions_string.append('-');
	}

	if ( f_info.permission(QFileInfo::WriteOther) == true ){
		permissions_string.append('w');
	} else {
		permissions_string.append('-');
	}

	if ( f_info.permission(QFileInfo::ExeOther) == true ){
		permissions_string.append('x');
	} else {
		permissions_string.append('-');
	}

	appendItem(GeneralGroup, "5_Permissions", QVariant(permissions_string) ); // informations about permissions are added


// set up size string
	size = (double)f_info.size();

	if ( size >= 1024.0 ){

		size /= 1024.0;
		if ( size >= 1024.0 ){
			size /= 1024.0;
			size_string.setNum(size, 'f', 1);

			if ( size_string.endsWith(".0") == true ){ // set precision to 0 if possible
				size_string.setLength(size_string.length() - 2);
			}

			size_string.append(" MB");
		} else {
			size_string.setNum(size, 'f', 1);

			if ( size_string.endsWith(".0") == true ){ // set precision to 0 if possible
				size_string.setLength(size_string.length() - 2);
			}

			size_string.append(" KB");
		}

	} else {

		size_string.setNum(size, 'f', 1);

		if ( size_string.endsWith(".0") == true ){ // set precision to 0 if possible
			size_string.setLength(size_string.length() - 2);
		}

		size_string.append(" B");

	}

	appendItem(GeneralGroup, "2_Size", QVariant(size_string) ); // add information about size



	return true;
}

#include "kfile_dvi.moc"
