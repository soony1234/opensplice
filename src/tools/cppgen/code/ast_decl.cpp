/*
 
COPYRIGHT
 
Copyright 1992, 1993, 1994 Sun Microsystems, Inc.  Printed in the United
States of America.  All Rights Reserved.
 
This product is protected by copyright and distributed under the following
license restricting its use.
 
The Interface Definition Language Compiler Front End (CFE) is made
available for your use provided that you include this license and copyright
notice on all media and documentation and the software program in which
this product is incorporated in whole or part. You may copy and extend
functionality (but may not remove functionality) of the Interface
Definition Language CFE without charge, but you are not authorized to
license or distribute it to anyone else except as part of a product or
program developed by you or with the express written consent of Sun
Microsystems, Inc. ("Sun").
 
The names of Sun Microsystems, Inc. and any of its subsidiaries or
affiliates may not be used in advertising or publicity pertaining to
distribution of Interface Definition Language CFE as permitted herein.
 
This license is effective until terminated by Sun for failure to comply
with this license.  Upon termination, you shall destroy or return all code
and documentation for the Interface Definition Language CFE.
 
INTERFACE DEFINITION LANGUAGE CFE IS PROVIDED AS IS WITH NO WARRANTIES OF
ANY KIND INCLUDING THE WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS
FOR A PARTICULAR PURPOSE, NONINFRINGEMENT, OR ARISING FROM A COURSE OF
DEALING, USAGE OR TRADE PRACTICE.
 
INTERFACE DEFINITION LANGUAGE CFE IS PROVIDED WITH NO SUPPORT AND WITHOUT
ANY OBLIGATION ON THE PART OF Sun OR ANY OF ITS SUBSIDIARIES OR AFFILIATES
TO ASSIST IN ITS USE, CORRECTION, MODIFICATION OR ENHANCEMENT.
 
SUN OR ANY OF ITS SUBSIDIARIES OR AFFILIATES SHALL HAVE NO LIABILITY WITH
RESPECT TO THE INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY
INTERFACE DEFINITION LANGUAGE CFE OR ANY PART THEREOF.
 
IN NO EVENT WILL SUN OR ANY OF ITS SUBSIDIARIES OR AFFILIATES BE LIABLE FOR
ANY LOST REVENUE OR PROFITS OR OTHER SPECIAL, INDIRECT AND CONSEQUENTIAL
DAMAGES, EVEN IF SUN HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 
Use, duplication, or disclosure by the government is subject to
restrictions as set forth in subparagraph (c)(1)(ii) of the Rights in
Technical Data and Computer Software clause at DFARS 252.227-7013 and FAR
52.227-19.
 
Sun, Sun Microsystems and the Sun logo are trademarks or registered
trademarks of Sun Microsystems, Inc.
 
SunSoft, Inc.  
2550 Garcia Avenue 
Mountain View, California  94043
 
NOTE:
 
SunOS, SunSoft, Sun, Solaris, Sun Microsystems or the Sun logo are
trademarks or registered trademarks of Sun Microsystems, Inc.
 
 */


/*
 * ast_decl.cc - Implementation of class AST_Decl
 *
 * AST_Decl is the base class for all AST nodes except AST_Expression.
 * AST_Decls have a node type (a value from the enum AST_Decl::NodeType)
 * a name (a UTL_ScopedName) and pragma info.
 * Additionally AST_Decl nodes record the scope of definition, the
 * file name in which they were defined, the line on which they were
 * defined in that file, and a boolean denoting whether this is the
 * main file or an #include'd file.
 */

#include <idl.h>
#include <idl_extern.h>

/*
 * Constructor(s) and destructor
 */

AST_Decl::AST_Decl ()
: 
   pd_imported (false),
   pd_in_main_file (false),
   pd_defined_in (NULL),
   pd_node_type (NT_module),
   pd_line (-1),
   pd_file_name (NULL),
   pd_name (NULL),
   pd_local_name (NULL),
   pd_added (false),
   pd_gen_any (false)
{}

AST_Decl::AST_Decl (NodeType nt, UTL_ScopedName *n)
: 
   pd_imported (idl_global->imported()),
   pd_in_main_file (idl_global->in_main_file()),
   pd_defined_in
   (
      idl_global->scopes()->depth() > 0
      ? idl_global->scopes()->top()
      : NULL
   ),
   pd_node_type (nt),
   pd_line (idl_global->lineno()),
   pd_file_name (idl_global->filename()),
   pd_local_name (n == NULL ? NULL : n->last_component()),
   pd_added (false),
   pd_gen_any (false)
{
   compute_full_name (n);
}

AST_Decl::AST_Decl (NodeType nt, UTL_ScopedName *n, const UTL_Pragmas &p)
: 
   pd_imported (idl_global->imported()),
   pd_in_main_file (idl_global->in_main_file()),
   pd_defined_in
   (
      idl_global->scopes()->depth() > 0
      ? idl_global->scopes()->top()
      : NULL
   ),
   pd_node_type (nt),
   pd_line (idl_global->lineno()),
   pd_file_name (idl_global->filename()),
   pd_local_name (n == NULL ? NULL : n->last_component()),
   pd_added (false),
   pd_gen_any (false),
   pd_pragmas (p)
{
   compute_full_name (n);
   pd_pragmas.set_name (pd_name);
}

/*
 * Private operations
 */

/*
 * Compute the full scoped name of an AST node
 */
void
AST_Decl::compute_full_name(UTL_ScopedName *n)
{
   UTL_ScopedName *cn = NULL;
   AST_Decl *d = NULL;

   /*
    * Initialize this name to NULL
    */
   pd_name = NULL;
   /*
    * Global scope?
    */

   if (defined_in() == NULL)
   {
      pd_name = n;
      return ;
   }

   /*
    * OK, not global. So copy name of containing scope, then
    * smash last cdr of copy with new component
    */
   d = ScopeAsDecl(defined_in());

   if (d != NULL)
      cn = d->name();

   if (cn != NULL)
      pd_name = (UTL_ScopedName *) cn->copy();

   if (pd_name == NULL)
      pd_name = new UTL_ScopedName(local_name(), NULL);
   else
      pd_name->nconc(new UTL_ScopedName(local_name(), NULL));
}

/*
 * Public operations
 */

/*
 * Return TRUE if one of my ancestor scopes is "s"
 * and FALSE otherwise
 */
bool AST_Decl::has_ancestor(AST_Decl *s)
{
   if (this == s)
      return true;

   if (pd_defined_in == NULL)
      return false;

   return ScopeAsDecl(pd_defined_in)->has_ancestor(s);
}

void AST_Decl::dump (ostream &o)
{
   pd_local_name->dump(o);
}

bool AST_Decl::imported ()
{
   return pd_imported;
}

void AST_Decl::set_imported (bool is_it)
{
   pd_imported = is_it;
}

bool AST_Decl::in_main_file ()
{
   return pd_in_main_file;
}

void AST_Decl::set_in_main_file (bool is_it)
{
   pd_in_main_file = is_it;
}

bool AST_Decl::added()
{
   return pd_added;
}

void AST_Decl::set_added (bool is_it)
{
   pd_added = is_it;
}

UTL_Scope * AST_Decl::defined_in ()
{
   return pd_defined_in;
}

void AST_Decl::set_defined_in (UTL_Scope *s)
{
   pd_defined_in = s;
}

AST_Decl::NodeType AST_Decl::node_type ()
{
   return pd_node_type;
}

long AST_Decl::line ()
{
   return pd_line;
}

void AST_Decl::set_line (long l)
{
   pd_line = l;
}

UTL_String * AST_Decl::file_name ()
{
   return pd_file_name;
}

void AST_Decl::set_file_name (UTL_String *s)
{
   pd_file_name = s;
}

UTL_ScopedName * AST_Decl::name ()
{
   return pd_name;
}

void AST_Decl::set_name (UTL_ScopedName *n)
{
   pd_name = n;

   if (n != NULL)
      pd_local_name = n->last_component();
}

Identifier * AST_Decl::local_name ()
{
   return pd_local_name;
}

bool AST_Decl::get_gen_any (void)
{
// printf ("AST_Decl::get_gen_any: %d\n", pd_gen_any || all_gen_any);
   return pd_gen_any || all_gen_any;
}

void AST_Decl::virt_set_gen_any (void)
{
   // Reimplemented by types that need to propagate any generation flag
}

// Whether any generation flag used for anything

bool AST_Decl::some_gen_any = false;

// Whether any generation flag used for everything

bool AST_Decl::all_gen_any = false;

void AST_Decl::set_gen_any (void)
{
   pd_gen_any = true;
   some_gen_any = true;
   virt_set_gen_any ();
}

IMPL_NARROW_METHODS0 (AST_Decl)
IMPL_NARROW_FROM_DECL (AST_Decl)
IMPL_NARROW_METHODS0 (COMMON_Base)
