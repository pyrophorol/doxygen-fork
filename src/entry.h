/******************************************************************************
 *
 *
 *
 * Copyright (C) 1997-2015 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby
 * granted. No representations are made about the suitability of this software
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#ifndef ENTRY_H
#define ENTRY_H

#include <vector>
#include <memory>
#include <sstream>

#include "types.h"
#include "arguments.h"
#include "reflist.h"
#include "textstream.h"
#include "configvalues.h"

class SectionInfo;
class FileDef;

/** This class stores information about an inheritance relation
 */
struct BaseInfo
{
  /*! Creates an object representing an inheritance relation */
  BaseInfo(const QCString &n,Protection p,Specifier v) :
    name(n),prot(p),virt(v) {}
  QCString   name; //!< the name of the base class
  Protection prot; //!< inheritance type
  Specifier  virt; //!< virtualness
};

/** This struct is used to capture the tag file information
 *  for an Entry.
 */
struct TagInfo
{
  QCString tagName;
  QCString fileName;
  QCString anchor;
};

/** Represents an unstructured piece of information, about an
 *  entity found in the sources.
 *
 *  parseMain() in scanner.l will generate a tree of these
 *  entries.
 */
class Entry
{
  public:

    /*! Kind of entries that are supported */
    enum Sections {
      CLASS_SEC        = 0x0000'0001,
      NAMESPACE_SEC    = 0x0000'0010,
      CONCEPT_SEC      = 0x0000'0020,
      COMPOUND_MASK    = CLASS_SEC,
      SCOPE_MASK       = COMPOUND_MASK | NAMESPACE_SEC,

      CLASSDOC_SEC     = 0x0000'0800,
      STRUCTDOC_SEC    = 0x0000'1000,
      UNIONDOC_SEC     = 0x0000'2000,
      EXCEPTIONDOC_SEC = 0x0000'4000,
      NAMESPACEDOC_SEC = 0x0000'8000,
      INTERFACEDOC_SEC = 0x0001'0000,
      PROTOCOLDOC_SEC  = 0x0002'0000,
      CATEGORYDOC_SEC  = 0x0004'0000,
      SERVICEDOC_SEC   = 0x0008'0000,
      SINGLETONDOC_SEC = 0x0010'0000,
      CONCEPTDOC_SEC   = 0x0020'0000,
      COMPOUNDDOC_MASK = CLASSDOC_SEC | STRUCTDOC_SEC | UNIONDOC_SEC |
                         INTERFACEDOC_SEC | EXCEPTIONDOC_SEC | PROTOCOLDOC_SEC |
                         CATEGORYDOC_SEC | SERVICEDOC_SEC | SINGLETONDOC_SEC,

      SOURCE_SEC       = 0x0040'0000,
      HEADER_SEC       = 0x0080'0000,
      FILE_MASK        = SOURCE_SEC | HEADER_SEC,

      ENUMDOC_SEC            = 0x0100'0000,
      ENUM_SEC               = 0x0200'0000,
      EMPTY_SEC              = 0x0300'0000,
      PAGEDOC_SEC            = 0x0400'0000,
      VARIABLE_SEC           = 0x0500'0000,
      FUNCTION_SEC           = 0x0600'0000,
      TYPEDEF_SEC            = 0x0700'0000,
      MEMBERDOC_SEC          = 0x0800'0000,
      OVERLOADDOC_SEC        = 0x0900'0000,
      EXAMPLE_SEC            = 0x0a00'0000,
      VARIABLEDOC_SEC        = 0x0b00'0000,
      FILEDOC_SEC            = 0x0c00'0000,
      DEFINEDOC_SEC          = 0x0d00'0000,
      INCLUDE_SEC            = 0x0e00'0000,
      DEFINE_SEC             = 0x0f00'0000,
      GROUPDOC_SEC           = 0x1000'0000,
      USINGDIR_SEC           = 0x1100'0000,
      MAINPAGEDOC_SEC        = 0x1200'0000,
      MEMBERGRP_SEC          = 0x1300'0000,
      USINGDECL_SEC          = 0x1400'0000,
      PACKAGE_SEC            = 0x1500'0000,
      PACKAGEDOC_SEC         = 0x1600'0000,
      OBJCIMPL_SEC           = 0x1700'0000,
      DIRDOC_SEC             = 0x1800'0000,
      EXPORTED_INTERFACE_SEC = 0x1900'0000,
      INCLUDED_SERVICE_SEC   = 0x1A00'0000,
      EXAMPLE_LINENO_SEC     = 0x1B00'0000,
      MODULEDOC_SEC          = 0x1C00'0000
    };

    enum GroupDocType
    {
      GROUPDOC_NORMAL,        //!< defgroup
      GROUPDOC_ADD,           //!< addtogroup
      GROUPDOC_WEAK           //!< weakgroup
    };                        //!< kind of group

    Entry();
    Entry(const Entry &);
   ~Entry();

    /*! Returns the parent for this Entry or 0 if this entry has no parent. */
    Entry *parent() const { return m_parent; }

    /*! Returns the list of children for this Entry
     *  @see addSubEntry() and removeSubEntry()
     */
    const std::vector< std::shared_ptr<Entry> > &children() const { return m_sublist; }

    /*! @name add entry as a child and pass ownership.
     *  @note This makes the entry passed invalid!
     *  @{
     */
    void moveToSubEntryAndKeep(Entry* e);
    void moveToSubEntryAndKeep(std::shared_ptr<Entry> e);
    /*! @} */

    /*! @name add entry as a child, pass ownership and reinitialize entry */
    void moveToSubEntryAndRefresh(Entry* &e);
    void moveToSubEntryAndRefresh(std::shared_ptr<Entry> &e);

    /*! make a copy of \a e and add it as a child to this entry */
    void copyToSubEntry (Entry* e);
    void copyToSubEntry (const std::shared_ptr<Entry> &e);

    /*! Removes entry \a e from the list of children.
     *  The entry will be deleted if found.
     */
    void removeSubEntry(const Entry *e);

    /*! Restore the state of this Entry to the default value it has
     *  at construction time.
     */
    void reset();

    void markAsProcessed() const { (const_cast<Entry*>(this))->section = Entry::EMPTY_SEC; }
    void setFileDef(FileDef *fd);
    FileDef *fileDef() const { return m_fileDef; }

    // identification
    int          section;     //!< entry type (see Sections);
    QCString	 type;        //!< member type
    QCString	 name;        //!< member name
    bool         hasTagInfo;  //!< is tag info valid
    TagInfo      tagInfoData; //!< tag file info data
    const TagInfo *tagInfo() const { return hasTagInfo ? &tagInfoData : 0; }

    // content
    Protection protection;    //!< class protection
    MethodTypes mtype;        //!< signal, slot, (dcop) method, or property?
    TypeSpecifier spec;       //!< class/member specifiers
    VhdlSpecifier vhdlSpec;   //!< VHDL specifiers
    int  initLines;           //!< define/variable initializer lines to show
    bool isStatic;            //!< static ?
    bool explicitExternal;    //!< explicitly defined as external?
    bool proto;               //!< prototype ?
    bool subGrouping;         //!< automatically group class members?
    bool callGraph;           //!< do we need to draw the call graph?
    bool callerGraph;         //!< do we need to draw the caller graph?
    bool referencedByRelation;//!< do we need to show the referenced by relation?
    bool referencesRelation;  //!< do we need to show the references relation?
    bool includeGraph;        //!< do we need to draw the include graph?
    bool includedByGraph;     //!< do we need to draw the included by graph?
    bool directoryGraph;      //!< do we need to draw the directory graph?
    bool collaborationGraph;  //!< do we need to draw the collaboration graph?
    CLASS_GRAPH_t inheritanceGraph; //!< type of inheritance graph?
    bool groupGraph;          //!< do we need to draw the group graph?
    bool exported;            //!< is the symbol exported from a C++20 module
    Specifier    virt;        //!< virtualness of the entry
    QCString     args;        //!< member argument string
    QCString     bitfields;   //!< member's bit fields
    ArgumentList argList;     //!< member arguments as a list
    ArgumentLists tArgLists;  //!< template argument declarations
    TextStream   program;     //!< the program text
    TextStream   initializer; //!< initial value (for variables)
    QCString     includeFile; //!< include file (2 arg of \\class, must be unique)
    QCString     includeName; //!< include name (3 arg of \\class)
    QCString     doc;         //!< documentation block (partly parsed)
    int          docLine;     //!< line number at which the documentation was found
    QCString     docFile;     //!< file in which the documentation was found
    QCString     brief;       //!< brief description (doc block)
    int          briefLine;   //!< line number at which the brief desc. was found
    QCString     briefFile;   //!< file in which the brief desc. was found
    QCString     inbodyDocs;  //!< documentation inside the body of a function
    int          inbodyLine;  //!< line number at which the body doc was found
    QCString     inbodyFile;  //!< file in which the body doc was found
    QCString     relates;     //!< related class (doc block)
    RelatesType  relatesType; //!< how relates is handled
    QCString     read;        //!< property read accessor
    QCString     write;       //!< property write accessor
    QCString     inside;      //!< name of the class in which documents are found
    QCString     exception;   //!< throw specification
    ArgumentList typeConstr;  //!< where clause (C#) for type constraints
    int          bodyLine;    //!< line number of the body in the source
    int          bodyColumn;  //!< column of the body in the source
    int          endBodyLine; //!< line number where the definition ends
    int          mGrpId;      //!< member group id
    std::vector<BaseInfo> extends; //!< list of base classes
    std::vector<Grouping> groups;  //!< list of groups this entry belongs to
    std::vector<const SectionInfo*> anchors; //!< list of anchors defined in this entry
    QCString	fileName;     //!< file this entry was extracted from
    int		startLine;    //!< start line of entry in the source
    int		startColumn;  //!< start column of entry in the source
    RefItemVector sli; //!< special lists (test/todo/bug/deprecated/..) this entry is in
    SrcLangExt  lang;         //!< programming language in which this entry was found
    bool        hidden;       //!< does this represent an entity that is hidden from the output
    bool        artificial;   //!< Artificially introduced item
    GroupDocType groupDocType;
    QCString    id;           //!< libclang id
    LocalToc    localToc;
    QCString    metaData;     //!< Slice metadata
    QCString    req;          //!< C++20 requires clause
    std::vector<std::string> qualifiers;  //!< qualifiers specified with the qualifier command

    /// return the command name used to define GROUPDOC_SEC
    const char *groupDocCmd() const
    {
      switch( groupDocType )
      {
        case GROUPDOC_NORMAL: return "\\defgroup";
        case GROUPDOC_ADD: return "\\addtogroup";
        case GROUPDOC_WEAK: return "\\weakgroup";
        default: return "unknown group command";
      }
    }
    Grouping::GroupPri_t groupingPri() const
    {
      if( section != GROUPDOC_SEC )
      {
        return Grouping::GROUPING_LOWEST;
      }
      switch( groupDocType )
      {
        case GROUPDOC_NORMAL: return Grouping::GROUPING_AUTO_DEF;
        case GROUPDOC_ADD:    return Grouping::GROUPING_AUTO_ADD;
        case GROUPDOC_WEAK:   return Grouping::GROUPING_AUTO_WEAK;
        default: return Grouping::GROUPING_LOWEST;
      }
    }

  private:
    Entry         *m_parent;    //!< parent node in the tree
    std::vector< std::shared_ptr<Entry> > m_sublist;
    Entry &operator=(const Entry &);
    FileDef       *m_fileDef;
};

typedef std::vector< std::shared_ptr<Entry> > EntryList;

#endif
