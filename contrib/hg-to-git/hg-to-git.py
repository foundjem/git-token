begin_unit
comment|'#! /usr/bin/python'
nl|'\n'
nl|'\n'
string|'""" hg-to-git.py - A Mercurial to GIT converter\n\n    Copyright (C)2007 Stelian Pop <stelian@popies.net>\n\n    This program is free software; you can redistribute it and/or modify\n    it under the terms of the GNU General Public License as published by\n    the Free Software Foundation; either version 2, or (at your option)\n    any later version.\n\n    This program is distributed in the hope that it will be useful,\n    but WITHOUT ANY WARRANTY; without even the implied warranty of\n    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n    GNU General Public License for more details.\n\n    You should have received a copy of the GNU General Public License\n    along with this program; if not, write to the Free Software\n    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n"""'
newline|'\n'
nl|'\n'
name|'import'
name|'os'
op|','
name|'os'
op|'.'
name|'path'
op|','
name|'sys'
newline|'\n'
name|'import'
name|'tempfile'
op|','
name|'popen2'
op|','
name|'pickle'
op|','
name|'getopt'
newline|'\n'
name|'import'
name|'re'
newline|'\n'
nl|'\n'
comment|'# Maps hg version -> git version'
nl|'\n'
DECL|variable|hgvers
name|'hgvers'
op|'='
op|'{'
op|'}'
newline|'\n'
comment|'# List of children for each hg revision'
nl|'\n'
DECL|variable|hgchildren
name|'hgchildren'
op|'='
op|'{'
op|'}'
newline|'\n'
comment|'# List of parents for each hg revision'
nl|'\n'
DECL|variable|hgparents
name|'hgparents'
op|'='
op|'{'
op|'}'
newline|'\n'
comment|'# Current branch for each hg revision'
nl|'\n'
DECL|variable|hgbranch
name|'hgbranch'
op|'='
op|'{'
op|'}'
newline|'\n'
comment|'# Number of new changesets converted from hg'
nl|'\n'
DECL|variable|hgnewcsets
name|'hgnewcsets'
op|'='
number|'0'
newline|'\n'
nl|'\n'
comment|'#------------------------------------------------------------------------------'
nl|'\n'
nl|'\n'
DECL|function|usage
name|'def'
name|'usage'
op|'('
op|')'
op|':'
newline|'\n'
nl|'\n'
indent|'        '
name|'print'
string|'"""\\\n%s: [OPTIONS] <hgprj>\n\noptions:\n    -s, --gitstate=FILE: name of the state to be saved/read\n                         for incrementals\n    -n, --nrepack=INT:   number of changesets that will trigger\n                         a repack (default=0, -1 to deactivate)\n    -v, --verbose:       be verbose\n\nrequired:\n    hgprj:  name of the HG project to import (directory)\n"""'
op|'%'
name|'sys'
op|'.'
name|'argv'
op|'['
number|'0'
op|']'
newline|'\n'
nl|'\n'
comment|'#------------------------------------------------------------------------------'
nl|'\n'
nl|'\n'
DECL|function|getgitenv
dedent|''
name|'def'
name|'getgitenv'
op|'('
name|'user'
op|','
name|'date'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'env'
op|'='
string|"''"
newline|'\n'
name|'elems'
op|'='
name|'re'
op|'.'
name|'compile'
op|'('
string|"'(.*?)\\s+<(.*)>'"
op|')'
op|'.'
name|'match'
op|'('
name|'user'
op|')'
newline|'\n'
name|'if'
name|'elems'
op|':'
newline|'\n'
indent|'        '
name|'env'
op|'+='
string|'\'export GIT_AUTHOR_NAME="%s" ;\''
op|'%'
name|'elems'
op|'.'
name|'group'
op|'('
number|'1'
op|')'
newline|'\n'
name|'env'
op|'+='
string|'\'export GIT_COMMITER_NAME="%s" ;\''
op|'%'
name|'elems'
op|'.'
name|'group'
op|'('
number|'1'
op|')'
newline|'\n'
name|'env'
op|'+='
string|'\'export GIT_AUTHOR_EMAIL="%s" ;\''
op|'%'
name|'elems'
op|'.'
name|'group'
op|'('
number|'2'
op|')'
newline|'\n'
name|'env'
op|'+='
string|'\'export GIT_COMMITER_EMAIL="%s" ;\''
op|'%'
name|'elems'
op|'.'
name|'group'
op|'('
number|'2'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'env'
op|'+='
string|'\'export GIT_AUTHOR_NAME="%s" ;\''
op|'%'
name|'user'
newline|'\n'
name|'env'
op|'+='
string|'\'export GIT_COMMITER_NAME="%s" ;\''
op|'%'
name|'user'
newline|'\n'
name|'env'
op|'+='
string|"'export GIT_AUTHOR_EMAIL= ;'"
newline|'\n'
name|'env'
op|'+='
string|"'export GIT_COMMITER_EMAIL= ;'"
newline|'\n'
nl|'\n'
dedent|''
name|'env'
op|'+='
string|'\'export GIT_AUTHOR_DATE="%s" ;\''
op|'%'
name|'date'
newline|'\n'
name|'env'
op|'+='
string|'\'export GIT_COMMITTER_DATE="%s" ;\''
op|'%'
name|'date'
newline|'\n'
name|'return'
name|'env'
newline|'\n'
nl|'\n'
comment|'#------------------------------------------------------------------------------'
nl|'\n'
nl|'\n'
DECL|variable|state
dedent|''
name|'state'
op|'='
string|"''"
newline|'\n'
DECL|variable|opt_nrepack
name|'opt_nrepack'
op|'='
number|'0'
newline|'\n'
DECL|variable|verbose
name|'verbose'
op|'='
name|'False'
newline|'\n'
nl|'\n'
name|'try'
op|':'
newline|'\n'
indent|'    '
name|'opts'
op|','
name|'args'
op|'='
name|'getopt'
op|'.'
name|'getopt'
op|'('
name|'sys'
op|'.'
name|'argv'
op|'['
number|'1'
op|':'
op|']'
op|','
string|"'s:t:n:v'"
op|','
op|'['
string|"'gitstate='"
op|','
string|"'tempdir='"
op|','
string|"'nrepack='"
op|','
string|"'verbose'"
op|']'
op|')'
newline|'\n'
name|'for'
name|'o'
op|','
name|'a'
name|'in'
name|'opts'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'o'
name|'in'
op|'('
string|"'-s'"
op|','
string|"'--gitstate'"
op|')'
op|':'
newline|'\n'
DECL|variable|state
indent|'            '
name|'state'
op|'='
name|'a'
newline|'\n'
DECL|variable|state
name|'state'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'abspath'
op|'('
name|'state'
op|')'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|"'-n'"
op|','
string|"'--nrepack'"
op|')'
op|':'
newline|'\n'
DECL|variable|opt_nrepack
indent|'            '
name|'opt_nrepack'
op|'='
name|'int'
op|'('
name|'a'
op|')'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|"'-v'"
op|','
string|"'--verbose'"
op|')'
op|':'
newline|'\n'
DECL|variable|verbose
indent|'            '
name|'verbose'
op|'='
name|'True'
newline|'\n'
dedent|''
dedent|''
name|'if'
name|'len'
op|'('
name|'args'
op|')'
op|'!='
number|'1'
op|':'
newline|'\n'
indent|'        '
name|'raise'
name|'Exception'
op|'('
string|"'params'"
op|')'
newline|'\n'
dedent|''
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'    '
name|'usage'
op|'('
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|hgprj
dedent|''
name|'hgprj'
op|'='
name|'args'
op|'['
number|'0'
op|']'
newline|'\n'
name|'os'
op|'.'
name|'chdir'
op|'('
name|'hgprj'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'state'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'os'
op|'.'
name|'path'
op|'.'
name|'exists'
op|'('
name|'state'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'verbose'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|"'State does exist, reading'"
newline|'\n'
DECL|variable|f
dedent|''
name|'f'
op|'='
name|'open'
op|'('
name|'state'
op|','
string|"'r'"
op|')'
newline|'\n'
DECL|variable|hgvers
name|'hgvers'
op|'='
name|'pickle'
op|'.'
name|'load'
op|'('
name|'f'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|"'State does not exist, first run'"
newline|'\n'
nl|'\n'
DECL|variable|sock
dedent|''
dedent|''
name|'sock'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'\'hg tip --template "{rev}"\''
op|')'
newline|'\n'
DECL|variable|tip
name|'tip'
op|'='
name|'sock'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
name|'if'
name|'sock'
op|'.'
name|'close'
op|'('
op|')'
op|':'
newline|'\n'
indent|'    '
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
dedent|''
name|'if'
name|'verbose'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|"'tip is'"
op|','
name|'tip'
newline|'\n'
nl|'\n'
comment|'# Calculate the branches'
nl|'\n'
dedent|''
name|'if'
name|'verbose'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|"'analysing the branches...'"
newline|'\n'
dedent|''
name|'hgchildren'
op|'['
string|'"0"'
op|']'
op|'='
op|'('
op|')'
newline|'\n'
name|'hgparents'
op|'['
string|'"0"'
op|']'
op|'='
op|'('
name|'None'
op|','
name|'None'
op|')'
newline|'\n'
name|'hgbranch'
op|'['
string|'"0"'
op|']'
op|'='
string|'"master"'
newline|'\n'
name|'for'
name|'cset'
name|'in'
name|'range'
op|'('
number|'1'
op|','
name|'int'
op|'('
name|'tip'
op|')'
op|'+'
number|'1'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'hgchildren'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
op|'('
op|')'
newline|'\n'
DECL|variable|prnts
name|'prnts'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'\'hg log -r %d --template "{parents}"\''
op|'%'
name|'cset'
op|')'
op|'.'
name|'read'
op|'('
op|')'
op|'.'
name|'strip'
op|'('
op|')'
op|'.'
name|'split'
op|'('
string|"' '"
op|')'
newline|'\n'
DECL|variable|prnts
name|'prnts'
op|'='
name|'map'
op|'('
name|'lambda'
name|'x'
op|':'
name|'x'
op|'['
op|':'
name|'x'
op|'.'
name|'find'
op|'('
string|"':'"
op|')'
op|']'
op|','
name|'prnts'
op|')'
newline|'\n'
name|'if'
name|'prnts'
op|'['
number|'0'
op|']'
op|'!='
string|"''"
op|':'
newline|'\n'
DECL|variable|parent
indent|'        '
name|'parent'
op|'='
name|'prnts'
op|'['
number|'0'
op|']'
op|'.'
name|'strip'
op|'('
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|parent
indent|'        '
name|'parent'
op|'='
name|'str'
op|'('
name|'cset'
op|'-'
number|'1'
op|')'
newline|'\n'
dedent|''
name|'hgchildren'
op|'['
name|'parent'
op|']'
op|'+='
op|'('
name|'str'
op|'('
name|'cset'
op|')'
op|','
op|')'
newline|'\n'
name|'if'
name|'len'
op|'('
name|'prnts'
op|')'
op|'>'
number|'1'
op|':'
newline|'\n'
DECL|variable|mparent
indent|'        '
name|'mparent'
op|'='
name|'prnts'
op|'['
number|'1'
op|']'
op|'.'
name|'strip'
op|'('
op|')'
newline|'\n'
name|'hgchildren'
op|'['
name|'mparent'
op|']'
op|'+='
op|'('
name|'str'
op|'('
name|'cset'
op|')'
op|','
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|mparent
indent|'        '
name|'mparent'
op|'='
name|'None'
newline|'\n'
nl|'\n'
dedent|''
name|'hgparents'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
op|'('
name|'parent'
op|','
name|'mparent'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'mparent'
op|':'
newline|'\n'
comment|"# For merge changesets, take either one, preferably the 'master' branch"
nl|'\n'
indent|'        '
name|'if'
name|'hgbranch'
op|'['
name|'mparent'
op|']'
op|'=='
string|"'master'"
op|':'
newline|'\n'
indent|'            '
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
string|"'master'"
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
name|'hgbranch'
op|'['
name|'parent'
op|']'
newline|'\n'
dedent|''
dedent|''
name|'else'
op|':'
newline|'\n'
comment|'# Normal changesets'
nl|'\n'
comment|'# For first children, take the parent branch, for the others create a new branch'
nl|'\n'
indent|'        '
name|'if'
name|'hgchildren'
op|'['
name|'parent'
op|']'
op|'['
number|'0'
op|']'
op|'=='
name|'str'
op|'('
name|'cset'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
name|'hgbranch'
op|'['
name|'parent'
op|']'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
string|'"branch-"'
op|'+'
name|'str'
op|'('
name|'cset'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
dedent|''
name|'if'
name|'not'
name|'hgvers'
op|'.'
name|'has_key'
op|'('
string|'"0"'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|"'creating repository'"
newline|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-init-db'"
op|')'
newline|'\n'
nl|'\n'
comment|'# loop through every hg changeset'
nl|'\n'
dedent|''
name|'for'
name|'cset'
name|'in'
name|'range'
op|'('
name|'int'
op|'('
name|'tip'
op|')'
op|'+'
number|'1'
op|')'
op|':'
newline|'\n'
nl|'\n'
comment|'# incremental, already seen'
nl|'\n'
indent|'    '
name|'if'
name|'hgvers'
op|'.'
name|'has_key'
op|'('
name|'str'
op|'('
name|'cset'
op|')'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'continue'
newline|'\n'
dedent|''
name|'hgnewcsets'
op|'+='
number|'1'
newline|'\n'
nl|'\n'
comment|'# get info'
nl|'\n'
DECL|variable|log_data
name|'log_data'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'\'hg log -r %d --template "{tags}\\n{date|date}\\n{author}\\n"\''
op|'%'
name|'cset'
op|')'
op|'.'
name|'readlines'
op|'('
op|')'
newline|'\n'
DECL|variable|tag
name|'tag'
op|'='
name|'log_data'
op|'['
number|'0'
op|']'
op|'.'
name|'strip'
op|'('
op|')'
newline|'\n'
DECL|variable|date
name|'date'
op|'='
name|'log_data'
op|'['
number|'1'
op|']'
op|'.'
name|'strip'
op|'('
op|')'
newline|'\n'
DECL|variable|user
name|'user'
op|'='
name|'log_data'
op|'['
number|'2'
op|']'
op|'.'
name|'strip'
op|'('
op|')'
newline|'\n'
DECL|variable|parent
name|'parent'
op|'='
name|'hgparents'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'['
number|'0'
op|']'
newline|'\n'
DECL|variable|mparent
name|'mparent'
op|'='
name|'hgparents'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'['
number|'1'
op|']'
newline|'\n'
nl|'\n'
comment|'#get comment'
nl|'\n'
op|'('
name|'fdcomment'
op|','
name|'filecomment'
op|')'
op|'='
name|'tempfile'
op|'.'
name|'mkstemp'
op|'('
op|')'
newline|'\n'
DECL|variable|csetcomment
name|'csetcomment'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|'\'hg log -r %d --template "{desc}"\''
op|'%'
name|'cset'
op|')'
op|'.'
name|'read'
op|'('
op|')'
op|'.'
name|'strip'
op|'('
op|')'
newline|'\n'
name|'os'
op|'.'
name|'write'
op|'('
name|'fdcomment'
op|','
name|'csetcomment'
op|')'
newline|'\n'
name|'os'
op|'.'
name|'close'
op|'('
name|'fdcomment'
op|')'
newline|'\n'
nl|'\n'
name|'print'
string|"'-----------------------------------------'"
newline|'\n'
name|'print'
string|"'cset:'"
op|','
name|'cset'
newline|'\n'
name|'print'
string|"'branch:'"
op|','
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
newline|'\n'
name|'print'
string|"'user:'"
op|','
name|'user'
newline|'\n'
name|'print'
string|"'date:'"
op|','
name|'date'
newline|'\n'
name|'print'
string|"'comment:'"
op|','
name|'csetcomment'
newline|'\n'
name|'if'
name|'parent'
op|':'
newline|'\n'
indent|'\t'
name|'print'
string|"'parent:'"
op|','
name|'parent'
newline|'\n'
dedent|''
name|'if'
name|'mparent'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|"'mparent:'"
op|','
name|'mparent'
newline|'\n'
dedent|''
name|'if'
name|'tag'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|"'tag:'"
op|','
name|'tag'
newline|'\n'
dedent|''
name|'print'
string|"'-----------------------------------------'"
newline|'\n'
nl|'\n'
comment|'# checkout the parent if necessary'
nl|'\n'
name|'if'
name|'cset'
op|'!='
number|'0'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'=='
string|'"branch-"'
op|'+'
name|'str'
op|'('
name|'cset'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|"'creating new branch'"
op|','
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
newline|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-checkout -b %s %s'"
op|'%'
op|'('
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|','
name|'hgvers'
op|'['
name|'parent'
op|']'
op|')'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'print'
string|"'checking out branch'"
op|','
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
newline|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-checkout %s'"
op|'%'
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|')'
newline|'\n'
nl|'\n'
comment|'# merge'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'mparent'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'hgbranch'
op|'['
name|'parent'
op|']'
op|'=='
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|':'
newline|'\n'
DECL|variable|otherbranch
indent|'            '
name|'otherbranch'
op|'='
name|'hgbranch'
op|'['
name|'mparent'
op|']'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|otherbranch
indent|'            '
name|'otherbranch'
op|'='
name|'hgbranch'
op|'['
name|'parent'
op|']'
newline|'\n'
dedent|''
name|'print'
string|"'merging'"
op|','
name|'otherbranch'
op|','
string|"'into'"
op|','
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
newline|'\n'
name|'os'
op|'.'
name|'system'
op|'('
name|'getgitenv'
op|'('
name|'user'
op|','
name|'date'
op|')'
op|'+'
string|'\'git-merge --no-commit -s ours "" %s %s\''
op|'%'
op|'('
name|'hgbranch'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|','
name|'otherbranch'
op|')'
op|')'
newline|'\n'
nl|'\n'
comment|'# remove everything except .git and .hg directories'
nl|'\n'
dedent|''
name|'os'
op|'.'
name|'system'
op|'('
string|'\'find . \\( -path "./.hg" -o -path "./.git" \\) -prune -o ! -name "." -print | xargs rm -rf\''
op|')'
newline|'\n'
nl|'\n'
comment|'# repopulate with checkouted files'
nl|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'hg update -C %d'"
op|'%'
name|'cset'
op|')'
newline|'\n'
nl|'\n'
comment|'# add new files'
nl|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-ls-files -x .hg --others | git-update-index --add --stdin'"
op|')'
newline|'\n'
comment|'# delete removed files'
nl|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-ls-files -x .hg --deleted | git-update-index --remove --stdin'"
op|')'
newline|'\n'
nl|'\n'
comment|'# commit'
nl|'\n'
name|'os'
op|'.'
name|'system'
op|'('
name|'getgitenv'
op|'('
name|'user'
op|','
name|'date'
op|')'
op|'+'
string|"'git commit --allow-empty -a -F %s'"
op|'%'
name|'filecomment'
op|')'
newline|'\n'
name|'os'
op|'.'
name|'unlink'
op|'('
name|'filecomment'
op|')'
newline|'\n'
nl|'\n'
comment|'# tag'
nl|'\n'
name|'if'
name|'tag'
name|'and'
name|'tag'
op|'!='
string|"'tip'"
op|':'
newline|'\n'
indent|'        '
name|'os'
op|'.'
name|'system'
op|'('
name|'getgitenv'
op|'('
name|'user'
op|','
name|'date'
op|')'
op|'+'
string|"'git-tag %s'"
op|'%'
name|'tag'
op|')'
newline|'\n'
nl|'\n'
comment|'# delete branch if not used anymore...'
nl|'\n'
dedent|''
name|'if'
name|'mparent'
name|'and'
name|'len'
op|'('
name|'hgchildren'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|'"Deleting unused branch:"'
op|','
name|'otherbranch'
newline|'\n'
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-branch -d %s'"
op|'%'
name|'otherbranch'
op|')'
newline|'\n'
nl|'\n'
comment|'# retrieve and record the version'
nl|'\n'
DECL|variable|vvv
dedent|''
name|'vvv'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|"'git-show --quiet --pretty=format:%H'"
op|')'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
name|'print'
string|"'record'"
op|','
name|'cset'
op|','
string|"'->'"
op|','
name|'vvv'
newline|'\n'
name|'hgvers'
op|'['
name|'str'
op|'('
name|'cset'
op|')'
op|']'
op|'='
name|'vvv'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'hgnewcsets'
op|'>='
name|'opt_nrepack'
name|'and'
name|'opt_nrepack'
op|'!='
op|'-'
number|'1'
op|':'
newline|'\n'
indent|'    '
name|'os'
op|'.'
name|'system'
op|'('
string|"'git-repack -a -d'"
op|')'
newline|'\n'
nl|'\n'
comment|'# write the state for incrementals'
nl|'\n'
dedent|''
name|'if'
name|'state'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'verbose'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|"'Writing state'"
newline|'\n'
DECL|variable|f
dedent|''
name|'f'
op|'='
name|'open'
op|'('
name|'state'
op|','
string|"'w'"
op|')'
newline|'\n'
name|'pickle'
op|'.'
name|'dump'
op|'('
name|'hgvers'
op|','
name|'f'
op|')'
newline|'\n'
nl|'\n'
comment|'# vim: et ts=8 sw=4 sts=4'
nl|'\n'
dedent|''
endmarker|''
end_unit
