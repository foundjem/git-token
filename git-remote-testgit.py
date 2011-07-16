begin_unit
comment|'#!/usr/bin/env python'
nl|'\n'
nl|'\n'
comment|'# hashlib is only available in python >= 2.5'
nl|'\n'
name|'try'
op|':'
newline|'\n'
indent|'    '
name|'import'
name|'hashlib'
newline|'\n'
DECL|variable|_digest
name|'_digest'
op|'='
name|'hashlib'
op|'.'
name|'sha1'
newline|'\n'
dedent|''
name|'except'
name|'ImportError'
op|':'
newline|'\n'
indent|'    '
name|'import'
name|'sha'
newline|'\n'
DECL|variable|_digest
name|'_digest'
op|'='
name|'sha'
op|'.'
name|'new'
newline|'\n'
dedent|''
name|'import'
name|'sys'
newline|'\n'
name|'import'
name|'os'
newline|'\n'
name|'sys'
op|'.'
name|'path'
op|'.'
name|'insert'
op|'('
number|'0'
op|','
name|'os'
op|'.'
name|'getenv'
op|'('
string|'"GITPYTHONLIB"'
op|','
string|'"."'
op|')'
op|')'
newline|'\n'
nl|'\n'
name|'from'
name|'git_remote_helpers'
op|'.'
name|'util'
name|'import'
name|'die'
op|','
name|'debug'
op|','
name|'warn'
newline|'\n'
name|'from'
name|'git_remote_helpers'
op|'.'
name|'git'
op|'.'
name|'repo'
name|'import'
name|'GitRepo'
newline|'\n'
name|'from'
name|'git_remote_helpers'
op|'.'
name|'git'
op|'.'
name|'exporter'
name|'import'
name|'GitExporter'
newline|'\n'
name|'from'
name|'git_remote_helpers'
op|'.'
name|'git'
op|'.'
name|'importer'
name|'import'
name|'GitImporter'
newline|'\n'
name|'from'
name|'git_remote_helpers'
op|'.'
name|'git'
op|'.'
name|'non_local'
name|'import'
name|'NonLocalGit'
newline|'\n'
nl|'\n'
DECL|function|get_repo
name|'def'
name|'get_repo'
op|'('
name|'alias'
op|','
name|'url'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Returns a git repository object initialized for usage.\n    """'
newline|'\n'
nl|'\n'
name|'repo'
op|'='
name|'GitRepo'
op|'('
name|'url'
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'get_revs'
op|'('
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'get_head'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'hasher'
op|'='
name|'_digest'
op|'('
op|')'
newline|'\n'
name|'hasher'
op|'.'
name|'update'
op|'('
name|'repo'
op|'.'
name|'path'
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'hash'
op|'='
name|'hasher'
op|'.'
name|'hexdigest'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'repo'
op|'.'
name|'get_base_path'
op|'='
name|'lambda'
name|'base'
op|':'
name|'os'
op|'.'
name|'path'
op|'.'
name|'join'
op|'('
nl|'\n'
name|'base'
op|','
string|"'info'"
op|','
string|"'fast-import'"
op|','
name|'repo'
op|'.'
name|'hash'
op|')'
newline|'\n'
nl|'\n'
name|'prefix'
op|'='
string|"'refs/testgit/%s/'"
op|'%'
name|'alias'
newline|'\n'
name|'debug'
op|'('
string|'"prefix: \'%s\'"'
op|','
name|'prefix'
op|')'
newline|'\n'
nl|'\n'
name|'repo'
op|'.'
name|'gitdir'
op|'='
name|'os'
op|'.'
name|'environ'
op|'['
string|'"GIT_DIR"'
op|']'
newline|'\n'
name|'repo'
op|'.'
name|'alias'
op|'='
name|'alias'
newline|'\n'
name|'repo'
op|'.'
name|'prefix'
op|'='
name|'prefix'
newline|'\n'
nl|'\n'
name|'repo'
op|'.'
name|'exporter'
op|'='
name|'GitExporter'
op|'('
name|'repo'
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'importer'
op|'='
name|'GitImporter'
op|'('
name|'repo'
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'non_local'
op|'='
name|'NonLocalGit'
op|'('
name|'repo'
op|')'
newline|'\n'
nl|'\n'
name|'return'
name|'repo'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|local_repo
dedent|''
name|'def'
name|'local_repo'
op|'('
name|'repo'
op|','
name|'path'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Returns a git repository object initalized for usage.\n    """'
newline|'\n'
nl|'\n'
name|'local'
op|'='
name|'GitRepo'
op|'('
name|'path'
op|')'
newline|'\n'
nl|'\n'
name|'local'
op|'.'
name|'non_local'
op|'='
name|'None'
newline|'\n'
name|'local'
op|'.'
name|'gitdir'
op|'='
name|'repo'
op|'.'
name|'gitdir'
newline|'\n'
name|'local'
op|'.'
name|'alias'
op|'='
name|'repo'
op|'.'
name|'alias'
newline|'\n'
name|'local'
op|'.'
name|'prefix'
op|'='
name|'repo'
op|'.'
name|'prefix'
newline|'\n'
name|'local'
op|'.'
name|'hash'
op|'='
name|'repo'
op|'.'
name|'hash'
newline|'\n'
name|'local'
op|'.'
name|'get_base_path'
op|'='
name|'repo'
op|'.'
name|'get_base_path'
newline|'\n'
name|'local'
op|'.'
name|'exporter'
op|'='
name|'GitExporter'
op|'('
name|'local'
op|')'
newline|'\n'
name|'local'
op|'.'
name|'importer'
op|'='
name|'GitImporter'
op|'('
name|'local'
op|')'
newline|'\n'
nl|'\n'
name|'return'
name|'local'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|do_capabilities
dedent|''
name|'def'
name|'do_capabilities'
op|'('
name|'repo'
op|','
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Prints the supported capabilities.\n    """'
newline|'\n'
nl|'\n'
name|'print'
string|'"import"'
newline|'\n'
name|'print'
string|'"export"'
newline|'\n'
name|'print'
string|'"refspec refs/heads/*:%s*"'
op|'%'
name|'repo'
op|'.'
name|'prefix'
newline|'\n'
nl|'\n'
name|'print'
comment|'# end capabilities'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|do_list
dedent|''
name|'def'
name|'do_list'
op|'('
name|'repo'
op|','
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Lists all known references.\n\n    Bug: This will always set the remote head to master for non-local\n    repositories, since we have no way of determining what the remote\n    head is at clone time.\n    """'
newline|'\n'
nl|'\n'
name|'for'
name|'ref'
name|'in'
name|'repo'
op|'.'
name|'revs'
op|':'
newline|'\n'
indent|'        '
name|'debug'
op|'('
string|'"? refs/heads/%s"'
op|','
name|'ref'
op|')'
newline|'\n'
name|'print'
string|'"? refs/heads/%s"'
op|'%'
name|'ref'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'repo'
op|'.'
name|'head'
op|':'
newline|'\n'
indent|'        '
name|'debug'
op|'('
string|'"@refs/heads/%s HEAD"'
op|'%'
name|'repo'
op|'.'
name|'head'
op|')'
newline|'\n'
name|'print'
string|'"@refs/heads/%s HEAD"'
op|'%'
name|'repo'
op|'.'
name|'head'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'debug'
op|'('
string|'"@refs/heads/master HEAD"'
op|')'
newline|'\n'
name|'print'
string|'"@refs/heads/master HEAD"'
newline|'\n'
nl|'\n'
dedent|''
name|'print'
comment|'# end list'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|update_local_repo
dedent|''
name|'def'
name|'update_local_repo'
op|'('
name|'repo'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Updates (or clones) a local repo.\n    """'
newline|'\n'
nl|'\n'
name|'if'
name|'repo'
op|'.'
name|'local'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'repo'
newline|'\n'
nl|'\n'
dedent|''
name|'path'
op|'='
name|'repo'
op|'.'
name|'non_local'
op|'.'
name|'clone'
op|'('
name|'repo'
op|'.'
name|'gitdir'
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'non_local'
op|'.'
name|'update'
op|'('
name|'repo'
op|'.'
name|'gitdir'
op|')'
newline|'\n'
name|'repo'
op|'='
name|'local_repo'
op|'('
name|'repo'
op|','
name|'path'
op|')'
newline|'\n'
name|'return'
name|'repo'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|do_import
dedent|''
name|'def'
name|'do_import'
op|'('
name|'repo'
op|','
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Exports a fast-import stream from testgit for git to import.\n    """'
newline|'\n'
nl|'\n'
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
name|'die'
op|'('
string|'"Import needs exactly one ref"'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'not'
name|'repo'
op|'.'
name|'gitdir'
op|':'
newline|'\n'
indent|'        '
name|'die'
op|'('
string|'"Need gitdir to import"'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'repo'
op|'='
name|'update_local_repo'
op|'('
name|'repo'
op|')'
newline|'\n'
name|'repo'
op|'.'
name|'exporter'
op|'.'
name|'export_repo'
op|'('
name|'repo'
op|'.'
name|'gitdir'
op|','
name|'args'
op|')'
newline|'\n'
nl|'\n'
name|'print'
string|'"done"'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|do_export
dedent|''
name|'def'
name|'do_export'
op|'('
name|'repo'
op|','
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Imports a fast-import stream from git to testgit.\n    """'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'repo'
op|'.'
name|'gitdir'
op|':'
newline|'\n'
indent|'        '
name|'die'
op|'('
string|'"Need gitdir to export"'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'dirname'
op|'='
name|'repo'
op|'.'
name|'get_base_path'
op|'('
name|'repo'
op|'.'
name|'gitdir'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'os'
op|'.'
name|'path'
op|'.'
name|'exists'
op|'('
name|'dirname'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'os'
op|'.'
name|'makedirs'
op|'('
name|'dirname'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'path'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'join'
op|'('
name|'dirname'
op|','
string|"'testgit.marks'"
op|')'
newline|'\n'
name|'print'
name|'path'
newline|'\n'
name|'if'
name|'os'
op|'.'
name|'path'
op|'.'
name|'exists'
op|'('
name|'path'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'print'
name|'path'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|'""'
newline|'\n'
dedent|''
name|'sys'
op|'.'
name|'stdout'
op|'.'
name|'flush'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'update_local_repo'
op|'('
name|'repo'
op|')'
newline|'\n'
name|'changed'
op|'='
name|'repo'
op|'.'
name|'importer'
op|'.'
name|'do_import'
op|'('
name|'repo'
op|'.'
name|'gitdir'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'repo'
op|'.'
name|'local'
op|':'
newline|'\n'
indent|'        '
name|'repo'
op|'.'
name|'non_local'
op|'.'
name|'push'
op|'('
name|'repo'
op|'.'
name|'gitdir'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'ref'
name|'in'
name|'changed'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|'"ok %s"'
op|'%'
name|'ref'
newline|'\n'
dedent|''
name|'print'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|variable|COMMANDS
dedent|''
name|'COMMANDS'
op|'='
op|'{'
nl|'\n'
string|"'capabilities'"
op|':'
name|'do_capabilities'
op|','
nl|'\n'
string|"'list'"
op|':'
name|'do_list'
op|','
nl|'\n'
string|"'import'"
op|':'
name|'do_import'
op|','
nl|'\n'
string|"'export'"
op|':'
name|'do_export'
op|','
nl|'\n'
op|'}'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|sanitize
name|'def'
name|'sanitize'
op|'('
name|'value'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Cleans up the url.\n    """'
newline|'\n'
nl|'\n'
name|'if'
name|'value'
op|'.'
name|'startswith'
op|'('
string|"'testgit::'"
op|')'
op|':'
newline|'\n'
indent|'        '
name|'value'
op|'='
name|'value'
op|'['
number|'9'
op|':'
op|']'
newline|'\n'
nl|'\n'
dedent|''
name|'return'
name|'value'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|read_one_line
dedent|''
name|'def'
name|'read_one_line'
op|'('
name|'repo'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Reads and processes one command.\n    """'
newline|'\n'
nl|'\n'
name|'line'
op|'='
name|'sys'
op|'.'
name|'stdin'
op|'.'
name|'readline'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'cmdline'
op|'='
name|'line'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'cmdline'
op|':'
newline|'\n'
indent|'        '
name|'warn'
op|'('
string|'"Unexpected EOF"'
op|')'
newline|'\n'
name|'return'
name|'False'
newline|'\n'
nl|'\n'
dedent|''
name|'cmdline'
op|'='
name|'cmdline'
op|'.'
name|'strip'
op|'('
op|')'
op|'.'
name|'split'
op|'('
op|')'
newline|'\n'
name|'if'
name|'not'
name|'cmdline'
op|':'
newline|'\n'
comment|"# Blank line means we're about to quit"
nl|'\n'
indent|'        '
name|'return'
name|'False'
newline|'\n'
nl|'\n'
dedent|''
name|'cmd'
op|'='
name|'cmdline'
op|'.'
name|'pop'
op|'('
number|'0'
op|')'
newline|'\n'
name|'debug'
op|'('
string|'"Got command \'%s\' with args \'%s\'"'
op|','
name|'cmd'
op|','
string|"' '"
op|'.'
name|'join'
op|'('
name|'cmdline'
op|')'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'cmd'
name|'not'
name|'in'
name|'COMMANDS'
op|':'
newline|'\n'
indent|'        '
name|'die'
op|'('
string|'"Unknown command, %s"'
op|','
name|'cmd'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'func'
op|'='
name|'COMMANDS'
op|'['
name|'cmd'
op|']'
newline|'\n'
name|'func'
op|'('
name|'repo'
op|','
name|'cmdline'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'stdout'
op|'.'
name|'flush'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'return'
name|'True'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|function|main
dedent|''
name|'def'
name|'main'
op|'('
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""Starts a new remote helper for the specified repository.\n    """'
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'args'
op|')'
op|'!='
number|'3'
op|':'
newline|'\n'
indent|'        '
name|'die'
op|'('
string|'"Expecting exactly three arguments."'
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
dedent|''
name|'if'
name|'os'
op|'.'
name|'getenv'
op|'('
string|'"GIT_DEBUG_TESTGIT"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'import'
name|'git_remote_helpers'
op|'.'
name|'util'
newline|'\n'
name|'git_remote_helpers'
op|'.'
name|'util'
op|'.'
name|'DEBUG'
op|'='
name|'True'
newline|'\n'
nl|'\n'
dedent|''
name|'alias'
op|'='
name|'sanitize'
op|'('
name|'args'
op|'['
number|'1'
op|']'
op|')'
newline|'\n'
name|'url'
op|'='
name|'sanitize'
op|'('
name|'args'
op|'['
number|'2'
op|']'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'alias'
op|'.'
name|'isalnum'
op|'('
op|')'
op|':'
newline|'\n'
indent|'        '
name|'warn'
op|'('
string|'"non-alnum alias \'%s\'"'
op|','
name|'alias'
op|')'
newline|'\n'
name|'alias'
op|'='
string|'"tmp"'
newline|'\n'
nl|'\n'
dedent|''
name|'args'
op|'['
number|'1'
op|']'
op|'='
name|'alias'
newline|'\n'
name|'args'
op|'['
number|'2'
op|']'
op|'='
name|'url'
newline|'\n'
nl|'\n'
name|'repo'
op|'='
name|'get_repo'
op|'('
name|'alias'
op|','
name|'url'
op|')'
newline|'\n'
nl|'\n'
name|'debug'
op|'('
string|'"Got arguments %s"'
op|','
name|'args'
op|'['
number|'1'
op|':'
op|']'
op|')'
newline|'\n'
nl|'\n'
name|'more'
op|'='
name|'True'
newline|'\n'
nl|'\n'
name|'while'
op|'('
name|'more'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'more'
op|'='
name|'read_one_line'
op|'('
name|'repo'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'__name__'
op|'=='
string|"'__main__'"
op|':'
newline|'\n'
indent|'    '
name|'sys'
op|'.'
name|'exit'
op|'('
name|'main'
op|'('
name|'sys'
op|'.'
name|'argv'
op|')'
op|')'
newline|'\n'
dedent|''
endmarker|''
end_unit
