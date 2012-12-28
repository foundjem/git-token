begin_unit
comment|'#!/usr/bin/env python'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# This tool is copyright (c) 2006, Sean Estabrooks.'
nl|'\n'
comment|'# It is released under the Gnu Public License, version 2.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# Import Perforce branches into Git repositories.'
nl|'\n'
comment|'# Checking out the files is done by calling the standard p4'
nl|'\n'
comment|'# client which you must have properly configured yourself'
nl|'\n'
comment|'#'
nl|'\n'
nl|'\n'
name|'import'
name|'marshal'
newline|'\n'
name|'import'
name|'os'
newline|'\n'
name|'import'
name|'sys'
newline|'\n'
name|'import'
name|'time'
newline|'\n'
name|'import'
name|'getopt'
newline|'\n'
nl|'\n'
name|'if'
name|'sys'
op|'.'
name|'hexversion'
op|'<'
number|'0x02020000'
op|':'
newline|'\n'
comment|'# The behavior of the marshal module changed significantly in 2.2'
nl|'\n'
indent|'   '
name|'sys'
op|'.'
name|'stderr'
op|'.'
name|'write'
op|'('
string|'"git-p4import.py: requires Python 2.2 or later.\\n"'
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
name|'from'
name|'signal'
name|'import'
name|'signal'
op|','
name|'SIGPIPE'
op|','
name|'SIGINT'
op|','
name|'SIG_DFL'
op|','
name|'default_int_handler'
newline|'\n'
nl|'\n'
name|'signal'
op|'('
name|'SIGPIPE'
op|','
name|'SIG_DFL'
op|')'
newline|'\n'
DECL|variable|s
name|'s'
op|'='
name|'signal'
op|'('
name|'SIGINT'
op|','
name|'SIG_DFL'
op|')'
newline|'\n'
name|'if'
name|'s'
op|'!='
name|'default_int_handler'
op|':'
newline|'\n'
indent|'   '
name|'signal'
op|'('
name|'SIGINT'
op|','
name|'s'
op|')'
newline|'\n'
nl|'\n'
DECL|function|die
dedent|''
name|'def'
name|'die'
op|'('
name|'msg'
op|','
op|'*'
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'for'
name|'a'
name|'in'
name|'args'
op|':'
newline|'\n'
indent|'        '
name|'msg'
op|'='
string|'"%s %s"'
op|'%'
op|'('
name|'msg'
op|','
name|'a'
op|')'
newline|'\n'
dedent|''
name|'print'
string|'"git-p4import fatal error:"'
op|','
name|'msg'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
nl|'\n'
DECL|function|usage
dedent|''
name|'def'
name|'usage'
op|'('
op|')'
op|':'
newline|'\n'
indent|'    '
name|'print'
string|'"USAGE: git-p4import [-q|-v]  [--authors=<file>]  [-t <timezone>]  [//p4repo/path <branch>]"'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|verbosity
dedent|''
name|'verbosity'
op|'='
number|'1'
newline|'\n'
DECL|variable|logfile
name|'logfile'
op|'='
string|'"/dev/null"'
newline|'\n'
DECL|variable|ignore_warnings
name|'ignore_warnings'
op|'='
name|'False'
newline|'\n'
DECL|variable|stitch
name|'stitch'
op|'='
number|'0'
newline|'\n'
DECL|variable|tagall
name|'tagall'
op|'='
name|'True'
newline|'\n'
nl|'\n'
DECL|function|report
name|'def'
name|'report'
op|'('
name|'level'
op|','
name|'msg'
op|','
op|'*'
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'global'
name|'verbosity'
newline|'\n'
name|'global'
name|'logfile'
newline|'\n'
name|'for'
name|'a'
name|'in'
name|'args'
op|':'
newline|'\n'
indent|'        '
name|'msg'
op|'='
string|'"%s %s"'
op|'%'
op|'('
name|'msg'
op|','
name|'a'
op|')'
newline|'\n'
dedent|''
name|'fd'
op|'='
name|'open'
op|'('
name|'logfile'
op|','
string|'"a"'
op|')'
newline|'\n'
name|'fd'
op|'.'
name|'writelines'
op|'('
name|'msg'
op|')'
newline|'\n'
name|'fd'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'if'
name|'level'
op|'<='
name|'verbosity'
op|':'
newline|'\n'
indent|'        '
name|'print'
name|'msg'
newline|'\n'
nl|'\n'
DECL|class|p4_command
dedent|''
dedent|''
name|'class'
name|'p4_command'
op|':'
newline|'\n'
DECL|member|__init__
indent|'    '
name|'def'
name|'__init__'
op|'('
name|'self'
op|','
name|'_repopath'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'global'
name|'logfile'
newline|'\n'
name|'self'
op|'.'
name|'userlist'
op|'='
op|'{'
op|'}'
newline|'\n'
name|'if'
name|'_repopath'
op|'['
op|'-'
number|'1'
op|']'
op|'=='
string|"'/'"
op|':'
newline|'\n'
indent|'                '
name|'self'
op|'.'
name|'repopath'
op|'='
name|'_repopath'
op|'['
op|':'
op|'-'
number|'1'
op|']'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'                '
name|'self'
op|'.'
name|'repopath'
op|'='
name|'_repopath'
newline|'\n'
dedent|''
name|'if'
name|'self'
op|'.'
name|'repopath'
op|'['
op|'-'
number|'4'
op|':'
op|']'
op|'!='
string|'"/..."'
op|':'
newline|'\n'
indent|'                '
name|'self'
op|'.'
name|'repopath'
op|'='
string|'"%s/..."'
op|'%'
name|'self'
op|'.'
name|'repopath'
newline|'\n'
dedent|''
name|'f'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|"'p4 -V 2>>%s'"
op|'%'
name|'logfile'
op|','
string|"'rb'"
op|')'
newline|'\n'
name|'a'
op|'='
name|'f'
op|'.'
name|'readlines'
op|'('
op|')'
newline|'\n'
name|'if'
name|'f'
op|'.'
name|'close'
op|'('
op|')'
op|':'
newline|'\n'
indent|'                '
name|'raise'
newline|'\n'
dedent|''
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'                '
name|'die'
op|'('
string|'"Could not find the \\"p4\\" command"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|p4
dedent|''
dedent|''
name|'def'
name|'p4'
op|'('
name|'self'
op|','
name|'cmd'
op|','
op|'*'
name|'args'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'global'
name|'logfile'
newline|'\n'
name|'cmd'
op|'='
string|'"%s %s"'
op|'%'
op|'('
name|'cmd'
op|','
string|"' '"
op|'.'
name|'join'
op|'('
name|'args'
op|')'
op|')'
newline|'\n'
name|'report'
op|'('
number|'2'
op|','
string|'"P4:"'
op|','
name|'cmd'
op|')'
newline|'\n'
name|'f'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|"'p4 -G %s 2>>%s'"
op|'%'
op|'('
name|'cmd'
op|','
name|'logfile'
op|')'
op|','
string|"'rb'"
op|')'
newline|'\n'
name|'list'
op|'='
op|'['
op|']'
newline|'\n'
name|'while'
number|'1'
op|':'
newline|'\n'
indent|'           '
name|'try'
op|':'
newline|'\n'
indent|'                '
name|'list'
op|'.'
name|'append'
op|'('
name|'marshal'
op|'.'
name|'load'
op|'('
name|'f'
op|')'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'EOFError'
op|':'
newline|'\n'
indent|'                '
name|'break'
newline|'\n'
dedent|''
dedent|''
name|'self'
op|'.'
name|'ret'
op|'='
name|'f'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'return'
name|'list'
newline|'\n'
nl|'\n'
DECL|member|sync
dedent|''
name|'def'
name|'sync'
op|'('
name|'self'
op|','
name|'id'
op|','
name|'force'
op|'='
name|'False'
op|','
name|'trick'
op|'='
name|'False'
op|','
name|'test'
op|'='
name|'False'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'force'
op|':'
newline|'\n'
indent|'            '
name|'ret'
op|'='
name|'self'
op|'.'
name|'p4'
op|'('
string|'"sync -f %s@%s"'
op|'%'
op|'('
name|'self'
op|'.'
name|'repopath'
op|','
name|'id'
op|')'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
dedent|''
name|'elif'
name|'trick'
op|':'
newline|'\n'
indent|'            '
name|'ret'
op|'='
name|'self'
op|'.'
name|'p4'
op|'('
string|'"sync -k %s@%s"'
op|'%'
op|'('
name|'self'
op|'.'
name|'repopath'
op|','
name|'id'
op|')'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
dedent|''
name|'elif'
name|'test'
op|':'
newline|'\n'
indent|'            '
name|'ret'
op|'='
name|'self'
op|'.'
name|'p4'
op|'('
string|'"sync -n %s@%s"'
op|'%'
op|'('
name|'self'
op|'.'
name|'repopath'
op|','
name|'id'
op|')'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'ret'
op|'='
name|'self'
op|'.'
name|'p4'
op|'('
string|'"sync    %s@%s"'
op|'%'
op|'('
name|'self'
op|'.'
name|'repopath'
op|','
name|'id'
op|')'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
dedent|''
name|'if'
name|'ret'
op|'['
string|"'code'"
op|']'
op|'=='
string|'"error"'
op|':'
newline|'\n'
indent|'             '
name|'data'
op|'='
name|'ret'
op|'['
string|"'data'"
op|']'
op|'.'
name|'upper'
op|'('
op|')'
newline|'\n'
name|'if'
name|'data'
op|'.'
name|'find'
op|'('
string|"'VIEW'"
op|')'
op|'>'
number|'0'
op|':'
newline|'\n'
indent|'                 '
name|'die'
op|'('
string|'"Perforce reports %s is not in client view"'
op|'%'
name|'self'
op|'.'
name|'repopath'
op|')'
newline|'\n'
dedent|''
name|'elif'
name|'data'
op|'.'
name|'find'
op|'('
string|"'UP-TO-DATE'"
op|')'
op|'<'
number|'0'
op|':'
newline|'\n'
indent|'                 '
name|'die'
op|'('
string|'"Could not sync files from perforce"'
op|','
name|'self'
op|'.'
name|'repopath'
op|')'
newline|'\n'
nl|'\n'
DECL|member|changes
dedent|''
dedent|''
dedent|''
name|'def'
name|'changes'
op|'('
name|'self'
op|','
name|'since'
op|'='
number|'0'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'list'
op|'='
op|'['
op|']'
newline|'\n'
name|'for'
name|'rec'
name|'in'
name|'self'
op|'.'
name|'p4'
op|'('
string|'"changes %s@%s,#head"'
op|'%'
op|'('
name|'self'
op|'.'
name|'repopath'
op|','
name|'since'
op|'+'
number|'1'
op|')'
op|')'
op|':'
newline|'\n'
indent|'                '
name|'list'
op|'.'
name|'append'
op|'('
name|'rec'
op|'['
string|"'change'"
op|']'
op|')'
newline|'\n'
dedent|''
name|'list'
op|'.'
name|'reverse'
op|'('
op|')'
newline|'\n'
name|'return'
name|'list'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'return'
op|'['
op|']'
newline|'\n'
nl|'\n'
DECL|member|authors
dedent|''
dedent|''
name|'def'
name|'authors'
op|'('
name|'self'
op|','
name|'filename'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'f'
op|'='
name|'open'
op|'('
name|'filename'
op|')'
newline|'\n'
name|'for'
name|'l'
name|'in'
name|'f'
op|'.'
name|'readlines'
op|'('
op|')'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'userlist'
op|'['
name|'l'
op|'['
op|':'
name|'l'
op|'.'
name|'find'
op|'('
string|"'='"
op|')'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
op|']'
op|'='
op|'('
name|'l'
op|'['
name|'l'
op|'.'
name|'find'
op|'('
string|"'='"
op|')'
op|'+'
number|'1'
op|':'
name|'l'
op|'.'
name|'find'
op|'('
string|"'<'"
op|')'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
op|','
name|'l'
op|'['
name|'l'
op|'.'
name|'find'
op|'('
string|"'<'"
op|')'
op|'+'
number|'1'
op|':'
name|'l'
op|'.'
name|'find'
op|'('
string|"'>'"
op|')'
op|']'
op|')'
newline|'\n'
dedent|''
name|'f'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'for'
name|'f'
op|','
name|'e'
name|'in'
name|'self'
op|'.'
name|'userlist'
op|'.'
name|'items'
op|'('
op|')'
op|':'
newline|'\n'
indent|'                '
name|'report'
op|'('
number|'2'
op|','
name|'f'
op|','
string|'":"'
op|','
name|'e'
op|'['
number|'0'
op|']'
op|','
string|'"  <"'
op|','
name|'e'
op|'['
number|'1'
op|']'
op|','
string|'">"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|_get_user
dedent|''
dedent|''
name|'def'
name|'_get_user'
op|'('
name|'self'
op|','
name|'id'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'not'
name|'self'
op|'.'
name|'userlist'
op|'.'
name|'has_key'
op|'('
name|'id'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'try'
op|':'
newline|'\n'
indent|'                '
name|'user'
op|'='
name|'self'
op|'.'
name|'p4'
op|'('
string|'"users"'
op|','
name|'id'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
name|'self'
op|'.'
name|'userlist'
op|'['
name|'id'
op|']'
op|'='
op|'('
name|'user'
op|'['
string|"'FullName'"
op|']'
op|','
name|'user'
op|'['
string|"'Email'"
op|']'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'                '
name|'self'
op|'.'
name|'userlist'
op|'['
name|'id'
op|']'
op|'='
op|'('
name|'id'
op|','
string|'""'
op|')'
newline|'\n'
dedent|''
dedent|''
name|'return'
name|'self'
op|'.'
name|'userlist'
op|'['
name|'id'
op|']'
newline|'\n'
nl|'\n'
DECL|member|_format_date
dedent|''
name|'def'
name|'_format_date'
op|'('
name|'self'
op|','
name|'ticks'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'symbol'
op|'='
string|"'+'"
newline|'\n'
name|'name'
op|'='
name|'time'
op|'.'
name|'tzname'
op|'['
number|'0'
op|']'
newline|'\n'
name|'offset'
op|'='
name|'time'
op|'.'
name|'timezone'
newline|'\n'
name|'if'
name|'ticks'
op|'['
number|'8'
op|']'
op|':'
newline|'\n'
indent|'            '
name|'name'
op|'='
name|'time'
op|'.'
name|'tzname'
op|'['
number|'1'
op|']'
newline|'\n'
name|'offset'
op|'='
name|'time'
op|'.'
name|'altzone'
newline|'\n'
dedent|''
name|'if'
name|'offset'
op|'<'
number|'0'
op|':'
newline|'\n'
indent|'            '
name|'offset'
op|'*='
op|'-'
number|'1'
newline|'\n'
name|'symbol'
op|'='
string|"'-'"
newline|'\n'
dedent|''
name|'localo'
op|'='
string|'"%s%02d%02d %s"'
op|'%'
op|'('
name|'symbol'
op|','
name|'offset'
op|'/'
number|'3600'
op|','
name|'offset'
op|'%'
number|'3600'
op|','
name|'name'
op|')'
newline|'\n'
name|'tickso'
op|'='
name|'time'
op|'.'
name|'strftime'
op|'('
string|'"%a %b %d %H:%M:%S %Y"'
op|','
name|'ticks'
op|')'
newline|'\n'
name|'return'
string|'"%s %s"'
op|'%'
op|'('
name|'tickso'
op|','
name|'localo'
op|')'
newline|'\n'
nl|'\n'
DECL|member|where
dedent|''
name|'def'
name|'where'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'return'
name|'self'
op|'.'
name|'p4'
op|'('
string|'"where %s"'
op|'%'
name|'self'
op|'.'
name|'repopath'
op|')'
op|'['
op|'-'
number|'1'
op|']'
op|'['
string|"'path'"
op|']'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'return'
string|'""'
newline|'\n'
nl|'\n'
DECL|member|describe
dedent|''
dedent|''
name|'def'
name|'describe'
op|'('
name|'self'
op|','
name|'num'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'desc'
op|'='
name|'self'
op|'.'
name|'p4'
op|'('
string|'"describe -s"'
op|','
name|'num'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
name|'self'
op|'.'
name|'msg'
op|'='
name|'desc'
op|'['
string|"'desc'"
op|']'
newline|'\n'
name|'self'
op|'.'
name|'author'
op|','
name|'self'
op|'.'
name|'email'
op|'='
name|'self'
op|'.'
name|'_get_user'
op|'('
name|'desc'
op|'['
string|"'user'"
op|']'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'date'
op|'='
name|'self'
op|'.'
name|'_format_date'
op|'('
name|'time'
op|'.'
name|'localtime'
op|'('
name|'long'
op|'('
name|'desc'
op|'['
string|"'time'"
op|']'
op|')'
op|')'
op|')'
newline|'\n'
name|'return'
name|'self'
newline|'\n'
nl|'\n'
DECL|class|git_command
dedent|''
dedent|''
name|'class'
name|'git_command'
op|':'
newline|'\n'
DECL|member|__init__
indent|'    '
name|'def'
name|'__init__'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'version'
op|'='
name|'self'
op|'.'
name|'git'
op|'('
string|'"--version"'
op|')'
op|'['
number|'0'
op|']'
op|'['
number|'12'
op|':'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'die'
op|'('
string|'"Could not find the \\"git\\" command"'
op|')'
newline|'\n'
dedent|''
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'gitdir'
op|'='
name|'self'
op|'.'
name|'get_single'
op|'('
string|'"rev-parse --git-dir"'
op|')'
newline|'\n'
name|'report'
op|'('
number|'2'
op|','
string|'"gdir:"'
op|','
name|'self'
op|'.'
name|'gitdir'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'die'
op|'('
string|'"Not a git repository... did you forget to \\"git init\\" ?"'
op|')'
newline|'\n'
dedent|''
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'cdup'
op|'='
name|'self'
op|'.'
name|'get_single'
op|'('
string|'"rev-parse --show-cdup"'
op|')'
newline|'\n'
name|'if'
name|'self'
op|'.'
name|'cdup'
op|'!='
string|'""'
op|':'
newline|'\n'
indent|'                '
name|'os'
op|'.'
name|'chdir'
op|'('
name|'self'
op|'.'
name|'cdup'
op|')'
newline|'\n'
dedent|''
name|'self'
op|'.'
name|'topdir'
op|'='
name|'os'
op|'.'
name|'getcwd'
op|'('
op|')'
newline|'\n'
name|'report'
op|'('
number|'2'
op|','
string|'"topdir:"'
op|','
name|'self'
op|'.'
name|'topdir'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'die'
op|'('
string|'"Could not find top git directory"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|git
dedent|''
dedent|''
name|'def'
name|'git'
op|'('
name|'self'
op|','
name|'cmd'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'global'
name|'logfile'
newline|'\n'
name|'report'
op|'('
number|'2'
op|','
string|'"GIT:"'
op|','
name|'cmd'
op|')'
newline|'\n'
name|'f'
op|'='
name|'os'
op|'.'
name|'popen'
op|'('
string|"'git %s 2>>%s'"
op|'%'
op|'('
name|'cmd'
op|','
name|'logfile'
op|')'
op|','
string|"'rb'"
op|')'
newline|'\n'
name|'r'
op|'='
name|'f'
op|'.'
name|'readlines'
op|'('
op|')'
newline|'\n'
name|'self'
op|'.'
name|'ret'
op|'='
name|'f'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'return'
name|'r'
newline|'\n'
nl|'\n'
DECL|member|get_single
dedent|''
name|'def'
name|'get_single'
op|'('
name|'self'
op|','
name|'cmd'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'self'
op|'.'
name|'git'
op|'('
name|'cmd'
op|')'
op|'['
number|'0'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
nl|'\n'
DECL|member|current_branch
dedent|''
name|'def'
name|'current_branch'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'testit'
op|'='
name|'self'
op|'.'
name|'git'
op|'('
string|'"rev-parse --verify HEAD"'
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
name|'return'
name|'self'
op|'.'
name|'git'
op|'('
string|'"symbolic-ref HEAD"'
op|')'
op|'['
number|'0'
op|']'
op|'['
number|'11'
op|':'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'return'
name|'None'
newline|'\n'
nl|'\n'
DECL|member|get_config
dedent|''
dedent|''
name|'def'
name|'get_config'
op|'('
name|'self'
op|','
name|'variable'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'return'
name|'self'
op|'.'
name|'git'
op|'('
string|'"config --get %s"'
op|'%'
name|'variable'
op|')'
op|'['
number|'0'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'return'
name|'None'
newline|'\n'
nl|'\n'
DECL|member|set_config
dedent|''
dedent|''
name|'def'
name|'set_config'
op|'('
name|'self'
op|','
name|'variable'
op|','
name|'value'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'git'
op|'('
string|'"config %s %s"'
op|'%'
op|'('
name|'variable'
op|','
name|'value'
op|')'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'die'
op|'('
string|'"Could not set %s to "'
op|'%'
name|'variable'
op|','
name|'value'
op|')'
newline|'\n'
nl|'\n'
DECL|member|make_tag
dedent|''
dedent|''
name|'def'
name|'make_tag'
op|'('
name|'self'
op|','
name|'name'
op|','
name|'head'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'git'
op|'('
string|'"tag -f %s %s"'
op|'%'
op|'('
name|'name'
op|','
name|'head'
op|')'
op|')'
newline|'\n'
nl|'\n'
DECL|member|top_change
dedent|''
name|'def'
name|'top_change'
op|'('
name|'self'
op|','
name|'branch'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'a'
op|'='
name|'self'
op|'.'
name|'get_single'
op|'('
string|'"name-rev --tags refs/heads/%s"'
op|'%'
name|'branch'
op|')'
newline|'\n'
name|'loc'
op|'='
name|'a'
op|'.'
name|'find'
op|'('
string|"' tags/'"
op|')'
op|'+'
number|'6'
newline|'\n'
name|'if'
name|'a'
op|'['
name|'loc'
op|':'
name|'loc'
op|'+'
number|'3'
op|']'
op|'!='
string|'"p4/"'
op|':'
newline|'\n'
indent|'                '
name|'raise'
newline|'\n'
dedent|''
name|'return'
name|'int'
op|'('
name|'a'
op|'['
name|'loc'
op|'+'
number|'3'
op|':'
op|']'
op|'['
op|':'
op|'-'
number|'2'
op|']'
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'            '
name|'return'
number|'0'
newline|'\n'
nl|'\n'
DECL|member|update_index
dedent|''
dedent|''
name|'def'
name|'update_index'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'git'
op|'('
string|'"ls-files -m -d -o -z | git update-index --add --remove -z --stdin"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|checkout
dedent|''
name|'def'
name|'checkout'
op|'('
name|'self'
op|','
name|'branch'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'git'
op|'('
string|'"checkout %s"'
op|'%'
name|'branch'
op|')'
newline|'\n'
nl|'\n'
DECL|member|repoint_head
dedent|''
name|'def'
name|'repoint_head'
op|'('
name|'self'
op|','
name|'branch'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'git'
op|'('
string|'"symbolic-ref HEAD refs/heads/%s"'
op|'%'
name|'branch'
op|')'
newline|'\n'
nl|'\n'
DECL|member|remove_files
dedent|''
name|'def'
name|'remove_files'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'git'
op|'('
string|'"ls-files | xargs rm"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|clean_directories
dedent|''
name|'def'
name|'clean_directories'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'git'
op|'('
string|'"clean -d"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|fresh_branch
dedent|''
name|'def'
name|'fresh_branch'
op|'('
name|'self'
op|','
name|'branch'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'report'
op|'('
number|'1'
op|','
string|'"Creating new branch"'
op|','
name|'branch'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'git'
op|'('
string|'"ls-files | xargs rm"'
op|')'
newline|'\n'
name|'os'
op|'.'
name|'remove'
op|'('
string|'".git/index"'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'repoint_head'
op|'('
name|'branch'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'git'
op|'('
string|'"clean -d"'
op|')'
newline|'\n'
nl|'\n'
DECL|member|basedir
dedent|''
name|'def'
name|'basedir'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'self'
op|'.'
name|'topdir'
newline|'\n'
nl|'\n'
DECL|member|commit
dedent|''
name|'def'
name|'commit'
op|'('
name|'self'
op|','
name|'author'
op|','
name|'email'
op|','
name|'date'
op|','
name|'msg'
op|','
name|'id'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'update_index'
op|'('
op|')'
newline|'\n'
name|'fd'
op|'='
name|'open'
op|'('
string|'".msg"'
op|','
string|'"w"'
op|')'
newline|'\n'
name|'fd'
op|'.'
name|'writelines'
op|'('
name|'msg'
op|')'
newline|'\n'
name|'fd'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
name|'try'
op|':'
newline|'\n'
indent|'                '
name|'current'
op|'='
name|'self'
op|'.'
name|'get_single'
op|'('
string|'"rev-parse --verify HEAD"'
op|')'
newline|'\n'
name|'head'
op|'='
string|'"-p HEAD"'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'                '
name|'current'
op|'='
string|'""'
newline|'\n'
name|'head'
op|'='
string|'""'
newline|'\n'
dedent|''
name|'tree'
op|'='
name|'self'
op|'.'
name|'get_single'
op|'('
string|'"write-tree"'
op|')'
newline|'\n'
name|'for'
name|'r'
op|','
name|'l'
name|'in'
op|'['
op|'('
string|"'DATE'"
op|','
name|'date'
op|')'
op|','
op|'('
string|"'NAME'"
op|','
name|'author'
op|')'
op|','
op|'('
string|"'EMAIL'"
op|','
name|'email'
op|')'
op|']'
op|':'
newline|'\n'
indent|'            '
name|'os'
op|'.'
name|'environ'
op|'['
string|"'GIT_AUTHOR_%s'"
op|'%'
name|'r'
op|']'
op|'='
name|'l'
newline|'\n'
name|'os'
op|'.'
name|'environ'
op|'['
string|"'GIT_COMMITTER_%s'"
op|'%'
name|'r'
op|']'
op|'='
name|'l'
newline|'\n'
dedent|''
name|'commit'
op|'='
name|'self'
op|'.'
name|'get_single'
op|'('
string|'"commit-tree %s %s < .msg"'
op|'%'
op|'('
name|'tree'
op|','
name|'head'
op|')'
op|')'
newline|'\n'
name|'os'
op|'.'
name|'remove'
op|'('
string|'".msg"'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'make_tag'
op|'('
string|'"p4/%s"'
op|'%'
name|'id'
op|','
name|'commit'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'git'
op|'('
string|'"update-ref HEAD %s %s"'
op|'%'
op|'('
name|'commit'
op|','
name|'current'
op|')'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
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
string|'"qhvt:"'
op|','
nl|'\n'
op|'['
string|'"authors="'
op|','
string|'"help"'
op|','
string|'"stitch="'
op|','
string|'"timezone="'
op|','
string|'"log="'
op|','
string|'"ignore"'
op|','
string|'"notags"'
op|']'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'getopt'
op|'.'
name|'GetoptError'
op|':'
newline|'\n'
indent|'    '
name|'usage'
op|'('
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'o'
op|','
name|'a'
name|'in'
name|'opts'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'o'
op|'=='
string|'"-q"'
op|':'
newline|'\n'
indent|'        '
name|'verbosity'
op|'='
number|'0'
newline|'\n'
dedent|''
name|'if'
name|'o'
op|'=='
string|'"-v"'
op|':'
newline|'\n'
indent|'        '
name|'verbosity'
op|'+='
number|'1'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|'"--log"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'logfile'
op|'='
name|'a'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|'"--notags"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'tagall'
op|'='
name|'False'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|'"-h"'
op|','
string|'"--help"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'usage'
op|'('
op|')'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|'"--ignore"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'ignore_warnings'
op|'='
name|'True'
newline|'\n'
nl|'\n'
DECL|variable|git
dedent|''
dedent|''
name|'git'
op|'='
name|'git_command'
op|'('
op|')'
newline|'\n'
DECL|variable|branch
name|'branch'
op|'='
name|'git'
op|'.'
name|'current_branch'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'for'
name|'o'
op|','
name|'a'
name|'in'
name|'opts'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'o'
name|'in'
op|'('
string|'"-t"'
op|','
string|'"--timezone"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'git'
op|'.'
name|'set_config'
op|'('
string|'"perforce.timezone"'
op|','
name|'a'
op|')'
newline|'\n'
dedent|''
name|'if'
name|'o'
name|'in'
op|'('
string|'"--stitch"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'git'
op|'.'
name|'set_config'
op|'('
string|'"perforce.%s.path"'
op|'%'
name|'branch'
op|','
name|'a'
op|')'
newline|'\n'
name|'stitch'
op|'='
number|'1'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'len'
op|'('
name|'args'
op|')'
op|'=='
number|'2'
op|':'
newline|'\n'
DECL|variable|branch
indent|'    '
name|'branch'
op|'='
name|'args'
op|'['
number|'1'
op|']'
newline|'\n'
name|'git'
op|'.'
name|'checkout'
op|'('
name|'branch'
op|')'
newline|'\n'
name|'if'
name|'branch'
op|'=='
name|'git'
op|'.'
name|'current_branch'
op|'('
op|')'
op|':'
newline|'\n'
indent|'        '
name|'die'
op|'('
string|'"Branch %s already exists!"'
op|'%'
name|'branch'
op|')'
newline|'\n'
dedent|''
name|'report'
op|'('
number|'1'
op|','
string|'"Setting perforce to "'
op|','
name|'args'
op|'['
number|'0'
op|']'
op|')'
newline|'\n'
name|'git'
op|'.'
name|'set_config'
op|'('
string|'"perforce.%s.path"'
op|'%'
name|'branch'
op|','
name|'args'
op|'['
number|'0'
op|']'
op|')'
newline|'\n'
dedent|''
name|'elif'
name|'len'
op|'('
name|'args'
op|')'
op|'!='
number|'0'
op|':'
newline|'\n'
indent|'    '
name|'die'
op|'('
string|'"You must specify the perforce //depot/path and git branch"'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|p4path
dedent|''
name|'p4path'
op|'='
name|'git'
op|'.'
name|'get_config'
op|'('
string|'"perforce.%s.path"'
op|'%'
name|'branch'
op|')'
newline|'\n'
name|'if'
name|'p4path'
op|'=='
name|'None'
op|':'
newline|'\n'
indent|'    '
name|'die'
op|'('
string|'"Do not know Perforce //depot/path for git branch"'
op|','
name|'branch'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|p4
dedent|''
name|'p4'
op|'='
name|'p4_command'
op|'('
name|'p4path'
op|')'
newline|'\n'
nl|'\n'
name|'for'
name|'o'
op|','
name|'a'
name|'in'
name|'opts'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'o'
name|'in'
op|'('
string|'"-a"'
op|','
string|'"--authors"'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'p4'
op|'.'
name|'authors'
op|'('
name|'a'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|localdir
dedent|''
dedent|''
name|'localdir'
op|'='
name|'git'
op|'.'
name|'basedir'
op|'('
op|')'
newline|'\n'
name|'if'
name|'p4'
op|'.'
name|'where'
op|'('
op|')'
op|'['
op|':'
name|'len'
op|'('
name|'localdir'
op|')'
op|']'
op|'!='
name|'localdir'
op|':'
newline|'\n'
indent|'    '
name|'report'
op|'('
number|'1'
op|','
string|'"**WARNING** Appears p4 client is misconfigured"'
op|')'
newline|'\n'
name|'report'
op|'('
number|'1'
op|','
string|'"   for sync from %s to %s"'
op|'%'
op|'('
name|'p4'
op|'.'
name|'repopath'
op|','
name|'localdir'
op|')'
op|')'
newline|'\n'
name|'if'
name|'ignore_warnings'
op|'!='
name|'True'
op|':'
newline|'\n'
indent|'        '
name|'die'
op|'('
string|'"Reconfigure or use \\"--ignore\\" on command line"'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'stitch'
op|'=='
number|'0'
op|':'
newline|'\n'
DECL|variable|top
indent|'    '
name|'top'
op|'='
name|'git'
op|'.'
name|'top_change'
op|'('
name|'branch'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|top
indent|'    '
name|'top'
op|'='
number|'0'
newline|'\n'
DECL|variable|changes
dedent|''
name|'changes'
op|'='
name|'p4'
op|'.'
name|'changes'
op|'('
name|'top'
op|')'
newline|'\n'
DECL|variable|count
name|'count'
op|'='
name|'len'
op|'('
name|'changes'
op|')'
newline|'\n'
name|'if'
name|'count'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'    '
name|'report'
op|'('
number|'1'
op|','
string|'"Already up to date..."'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'0'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|ptz
dedent|''
name|'ptz'
op|'='
name|'git'
op|'.'
name|'get_config'
op|'('
string|'"perforce.timezone"'
op|')'
newline|'\n'
name|'if'
name|'ptz'
op|':'
newline|'\n'
indent|'    '
name|'report'
op|'('
number|'1'
op|','
string|'"Setting timezone to"'
op|','
name|'ptz'
op|')'
newline|'\n'
name|'os'
op|'.'
name|'environ'
op|'['
string|"'TZ'"
op|']'
op|'='
name|'ptz'
newline|'\n'
name|'time'
op|'.'
name|'tzset'
op|'('
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'stitch'
op|'=='
number|'1'
op|':'
newline|'\n'
indent|'    '
name|'git'
op|'.'
name|'remove_files'
op|'('
op|')'
newline|'\n'
name|'git'
op|'.'
name|'clean_directories'
op|'('
op|')'
newline|'\n'
name|'p4'
op|'.'
name|'sync'
op|'('
name|'changes'
op|'['
number|'0'
op|']'
op|','
name|'force'
op|'='
name|'True'
op|')'
newline|'\n'
dedent|''
name|'elif'
name|'top'
op|'=='
number|'0'
name|'and'
name|'branch'
op|'!='
name|'git'
op|'.'
name|'current_branch'
op|'('
op|')'
op|':'
newline|'\n'
indent|'    '
name|'p4'
op|'.'
name|'sync'
op|'('
name|'changes'
op|'['
number|'0'
op|']'
op|','
name|'test'
op|'='
name|'True'
op|')'
newline|'\n'
name|'report'
op|'('
number|'1'
op|','
string|'"Creating new initial commit"'
op|')'
op|';'
newline|'\n'
name|'git'
op|'.'
name|'fresh_branch'
op|'('
name|'branch'
op|')'
newline|'\n'
name|'p4'
op|'.'
name|'sync'
op|'('
name|'changes'
op|'['
number|'0'
op|']'
op|','
name|'force'
op|'='
name|'True'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'    '
name|'p4'
op|'.'
name|'sync'
op|'('
name|'changes'
op|'['
number|'0'
op|']'
op|','
name|'trick'
op|'='
name|'True'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'report'
op|'('
number|'1'
op|','
string|'"processing %s changes from p4 (%s) to git (%s)"'
op|'%'
op|'('
name|'count'
op|','
name|'p4'
op|'.'
name|'repopath'
op|','
name|'branch'
op|')'
op|')'
newline|'\n'
name|'for'
name|'id'
name|'in'
name|'changes'
op|':'
newline|'\n'
indent|'    '
name|'report'
op|'('
number|'1'
op|','
string|'"Importing changeset"'
op|','
name|'id'
op|')'
newline|'\n'
DECL|variable|change
name|'change'
op|'='
name|'p4'
op|'.'
name|'describe'
op|'('
name|'id'
op|')'
newline|'\n'
name|'p4'
op|'.'
name|'sync'
op|'('
name|'id'
op|')'
newline|'\n'
name|'if'
name|'tagall'
op|':'
newline|'\n'
indent|'            '
name|'git'
op|'.'
name|'commit'
op|'('
name|'change'
op|'.'
name|'author'
op|','
name|'change'
op|'.'
name|'email'
op|','
name|'change'
op|'.'
name|'date'
op|','
name|'change'
op|'.'
name|'msg'
op|','
name|'id'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'git'
op|'.'
name|'commit'
op|'('
name|'change'
op|'.'
name|'author'
op|','
name|'change'
op|'.'
name|'email'
op|','
name|'change'
op|'.'
name|'date'
op|','
name|'change'
op|'.'
name|'msg'
op|','
string|'"import"'
op|')'
newline|'\n'
dedent|''
name|'if'
name|'stitch'
op|'=='
number|'1'
op|':'
newline|'\n'
indent|'        '
name|'git'
op|'.'
name|'clean_directories'
op|'('
op|')'
newline|'\n'
name|'stitch'
op|'='
number|'0'
newline|'\n'
dedent|''
dedent|''
endmarker|''
end_unit
