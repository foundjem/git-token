begin_unit
comment|'#'
nl|'\n'
comment|'# Copyright (C) 2005 Fredrik Kuivinen'
nl|'\n'
comment|'#'
nl|'\n'
nl|'\n'
name|'import'
name|'sys'
op|','
name|'re'
op|','
name|'os'
op|','
name|'traceback'
newline|'\n'
name|'from'
name|'sets'
name|'import'
name|'Set'
newline|'\n'
nl|'\n'
DECL|function|die
name|'def'
name|'die'
op|'('
op|'*'
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'printList'
op|'('
name|'args'
op|','
name|'sys'
op|'.'
name|'stderr'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'2'
op|')'
newline|'\n'
nl|'\n'
DECL|function|printList
dedent|''
name|'def'
name|'printList'
op|'('
name|'list'
op|','
name|'file'
op|'='
name|'sys'
op|'.'
name|'stdout'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'for'
name|'x'
name|'in'
name|'list'
op|':'
newline|'\n'
indent|'        '
name|'file'
op|'.'
name|'write'
op|'('
name|'str'
op|'('
name|'x'
op|')'
op|')'
newline|'\n'
name|'file'
op|'.'
name|'write'
op|'('
string|"' '"
op|')'
newline|'\n'
dedent|''
name|'file'
op|'.'
name|'write'
op|'('
string|"'\\n'"
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'import'
name|'subprocess'
newline|'\n'
nl|'\n'
comment|'# Debugging machinery'
nl|'\n'
comment|'# -------------------'
nl|'\n'
nl|'\n'
DECL|variable|DEBUG
name|'DEBUG'
op|'='
number|'0'
newline|'\n'
DECL|variable|functionsToDebug
name|'functionsToDebug'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
nl|'\n'
DECL|function|addDebug
name|'def'
name|'addDebug'
op|'('
name|'func'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'type'
op|'('
name|'func'
op|')'
op|'=='
name|'str'
op|':'
newline|'\n'
indent|'        '
name|'functionsToDebug'
op|'.'
name|'add'
op|'('
name|'func'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'functionsToDebug'
op|'.'
name|'add'
op|'('
name|'func'
op|'.'
name|'func_name'
op|')'
newline|'\n'
nl|'\n'
DECL|function|debug
dedent|''
dedent|''
name|'def'
name|'debug'
op|'('
op|'*'
name|'args'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'if'
name|'DEBUG'
op|':'
newline|'\n'
indent|'        '
name|'funcName'
op|'='
name|'traceback'
op|'.'
name|'extract_stack'
op|'('
op|')'
op|'['
op|'-'
number|'2'
op|']'
op|'['
number|'2'
op|']'
newline|'\n'
name|'if'
name|'funcName'
name|'in'
name|'functionsToDebug'
op|':'
newline|'\n'
indent|'            '
name|'printList'
op|'('
name|'args'
op|')'
newline|'\n'
nl|'\n'
comment|'# Program execution'
nl|'\n'
comment|'# -----------------'
nl|'\n'
nl|'\n'
DECL|class|ProgramError
dedent|''
dedent|''
dedent|''
name|'class'
name|'ProgramError'
op|'('
name|'Exception'
op|')'
op|':'
newline|'\n'
DECL|member|__init__
indent|'    '
name|'def'
name|'__init__'
op|'('
name|'self'
op|','
name|'progStr'
op|','
name|'error'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'progStr'
op|'='
name|'progStr'
newline|'\n'
name|'self'
op|'.'
name|'error'
op|'='
name|'error'
newline|'\n'
nl|'\n'
DECL|member|__str__
dedent|''
name|'def'
name|'__str__'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'self'
op|'.'
name|'progStr'
op|'+'
string|"': '"
op|'+'
name|'self'
op|'.'
name|'error'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'addDebug'
op|'('
string|"'runProgram'"
op|')'
newline|'\n'
DECL|function|runProgram
name|'def'
name|'runProgram'
op|'('
name|'prog'
op|','
name|'input'
op|'='
name|'None'
op|','
name|'returnCode'
op|'='
name|'False'
op|','
name|'env'
op|'='
name|'None'
op|','
name|'pipeOutput'
op|'='
name|'True'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'debug'
op|'('
string|"'runProgram prog:'"
op|','
name|'str'
op|'('
name|'prog'
op|')'
op|','
string|"'input:'"
op|','
name|'str'
op|'('
name|'input'
op|')'
op|')'
newline|'\n'
name|'if'
name|'type'
op|'('
name|'prog'
op|')'
name|'is'
name|'str'
op|':'
newline|'\n'
indent|'        '
name|'progStr'
op|'='
name|'prog'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'progStr'
op|'='
string|"' '"
op|'.'
name|'join'
op|'('
name|'prog'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'try'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'pipeOutput'
op|':'
newline|'\n'
indent|'            '
name|'stderr'
op|'='
name|'subprocess'
op|'.'
name|'STDOUT'
newline|'\n'
name|'stdout'
op|'='
name|'subprocess'
op|'.'
name|'PIPE'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'stderr'
op|'='
name|'None'
newline|'\n'
name|'stdout'
op|'='
name|'None'
newline|'\n'
dedent|''
name|'pop'
op|'='
name|'subprocess'
op|'.'
name|'Popen'
op|'('
name|'prog'
op|','
nl|'\n'
name|'shell'
op|'='
name|'type'
op|'('
name|'prog'
op|')'
name|'is'
name|'str'
op|','
nl|'\n'
name|'stderr'
op|'='
name|'stderr'
op|','
nl|'\n'
name|'stdout'
op|'='
name|'stdout'
op|','
nl|'\n'
name|'stdin'
op|'='
name|'subprocess'
op|'.'
name|'PIPE'
op|','
nl|'\n'
name|'env'
op|'='
name|'env'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'OSError'
op|','
name|'e'
op|':'
newline|'\n'
indent|'        '
name|'debug'
op|'('
string|"'strerror:'"
op|','
name|'e'
op|'.'
name|'strerror'
op|')'
newline|'\n'
name|'raise'
name|'ProgramError'
op|'('
name|'progStr'
op|','
name|'e'
op|'.'
name|'strerror'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'input'
op|'!='
name|'None'
op|':'
newline|'\n'
indent|'        '
name|'pop'
op|'.'
name|'stdin'
op|'.'
name|'write'
op|'('
name|'input'
op|')'
newline|'\n'
dedent|''
name|'pop'
op|'.'
name|'stdin'
op|'.'
name|'close'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'pipeOutput'
op|':'
newline|'\n'
indent|'        '
name|'out'
op|'='
name|'pop'
op|'.'
name|'stdout'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'out'
op|'='
string|"''"
newline|'\n'
nl|'\n'
dedent|''
name|'code'
op|'='
name|'pop'
op|'.'
name|'wait'
op|'('
op|')'
newline|'\n'
name|'if'
name|'returnCode'
op|':'
newline|'\n'
indent|'        '
name|'ret'
op|'='
op|'['
name|'out'
op|','
name|'code'
op|']'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'ret'
op|'='
name|'out'
newline|'\n'
dedent|''
name|'if'
name|'code'
op|'!='
number|'0'
name|'and'
name|'not'
name|'returnCode'
op|':'
newline|'\n'
indent|'        '
name|'debug'
op|'('
string|"'error output:'"
op|','
name|'out'
op|')'
newline|'\n'
name|'debug'
op|'('
string|"'prog:'"
op|','
name|'prog'
op|')'
newline|'\n'
name|'raise'
name|'ProgramError'
op|'('
name|'progStr'
op|','
name|'out'
op|')'
newline|'\n'
comment|"#    debug('output:', out.replace('\\0', '\\n'))"
nl|'\n'
dedent|''
name|'return'
name|'ret'
newline|'\n'
nl|'\n'
comment|'# Code for computing common ancestors'
nl|'\n'
comment|'# -----------------------------------'
nl|'\n'
nl|'\n'
DECL|variable|currentId
dedent|''
name|'currentId'
op|'='
number|'0'
newline|'\n'
DECL|function|getUniqueId
name|'def'
name|'getUniqueId'
op|'('
op|')'
op|':'
newline|'\n'
indent|'    '
name|'global'
name|'currentId'
newline|'\n'
name|'currentId'
op|'+='
number|'1'
newline|'\n'
name|'return'
name|'currentId'
newline|'\n'
nl|'\n'
comment|"# The 'virtual' commit objects have SHAs which are integers"
nl|'\n'
DECL|variable|shaRE
dedent|''
name|'shaRE'
op|'='
name|'re'
op|'.'
name|'compile'
op|'('
string|"'^[0-9a-f]{40}$'"
op|')'
newline|'\n'
DECL|function|isSha
name|'def'
name|'isSha'
op|'('
name|'obj'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'return'
op|'('
name|'type'
op|'('
name|'obj'
op|')'
name|'is'
name|'str'
name|'and'
name|'bool'
op|'('
name|'shaRE'
op|'.'
name|'match'
op|'('
name|'obj'
op|')'
op|')'
op|')'
name|'or'
op|'('
name|'type'
op|'('
name|'obj'
op|')'
name|'is'
name|'int'
name|'and'
name|'obj'
op|'>='
number|'1'
op|')'
newline|'\n'
nl|'\n'
DECL|class|Commit
dedent|''
name|'class'
name|'Commit'
op|'('
name|'object'
op|')'
op|':'
newline|'\n'
DECL|variable|__slots__
indent|'    '
name|'__slots__'
op|'='
op|'['
string|"'parents'"
op|','
string|"'firstLineMsg'"
op|','
string|"'children'"
op|','
string|"'_tree'"
op|','
string|"'sha'"
op|','
nl|'\n'
string|"'virtual'"
op|']'
newline|'\n'
nl|'\n'
DECL|member|__init__
name|'def'
name|'__init__'
op|'('
name|'self'
op|','
name|'sha'
op|','
name|'parents'
op|','
name|'tree'
op|'='
name|'None'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'parents'
op|'='
name|'parents'
newline|'\n'
name|'self'
op|'.'
name|'firstLineMsg'
op|'='
name|'None'
newline|'\n'
name|'self'
op|'.'
name|'children'
op|'='
op|'['
op|']'
newline|'\n'
nl|'\n'
name|'if'
name|'tree'
op|':'
newline|'\n'
indent|'            '
name|'tree'
op|'='
name|'tree'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
name|'assert'
op|'('
name|'isSha'
op|'('
name|'tree'
op|')'
op|')'
newline|'\n'
dedent|''
name|'self'
op|'.'
name|'_tree'
op|'='
name|'tree'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'sha'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'sha'
op|'='
name|'getUniqueId'
op|'('
op|')'
newline|'\n'
name|'self'
op|'.'
name|'virtual'
op|'='
name|'True'
newline|'\n'
name|'self'
op|'.'
name|'firstLineMsg'
op|'='
string|"'virtual commit'"
newline|'\n'
name|'assert'
op|'('
name|'isSha'
op|'('
name|'tree'
op|')'
op|')'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'self'
op|'.'
name|'virtual'
op|'='
name|'False'
newline|'\n'
name|'self'
op|'.'
name|'sha'
op|'='
name|'sha'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
dedent|''
name|'assert'
op|'('
name|'isSha'
op|'('
name|'self'
op|'.'
name|'sha'
op|')'
op|')'
newline|'\n'
nl|'\n'
DECL|member|tree
dedent|''
name|'def'
name|'tree'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'getInfo'
op|'('
op|')'
newline|'\n'
name|'assert'
op|'('
name|'self'
op|'.'
name|'_tree'
op|'!='
name|'None'
op|')'
newline|'\n'
name|'return'
name|'self'
op|'.'
name|'_tree'
newline|'\n'
nl|'\n'
DECL|member|shortInfo
dedent|''
name|'def'
name|'shortInfo'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'self'
op|'.'
name|'getInfo'
op|'('
op|')'
newline|'\n'
name|'return'
name|'str'
op|'('
name|'self'
op|'.'
name|'sha'
op|')'
op|'+'
string|"' '"
op|'+'
name|'self'
op|'.'
name|'firstLineMsg'
newline|'\n'
nl|'\n'
DECL|member|__str__
dedent|''
name|'def'
name|'__str__'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'self'
op|'.'
name|'shortInfo'
op|'('
op|')'
newline|'\n'
nl|'\n'
DECL|member|getInfo
dedent|''
name|'def'
name|'getInfo'
op|'('
name|'self'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'self'
op|'.'
name|'virtual'
name|'or'
name|'self'
op|'.'
name|'firstLineMsg'
op|'!='
name|'None'
op|':'
newline|'\n'
indent|'            '
name|'return'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'info'
op|'='
name|'runProgram'
op|'('
op|'['
string|"'git-cat-file'"
op|','
string|"'commit'"
op|','
name|'self'
op|'.'
name|'sha'
op|']'
op|')'
newline|'\n'
name|'info'
op|'='
name|'info'
op|'.'
name|'split'
op|'('
string|"'\\n'"
op|')'
newline|'\n'
name|'msg'
op|'='
name|'False'
newline|'\n'
name|'for'
name|'l'
name|'in'
name|'info'
op|':'
newline|'\n'
indent|'                '
name|'if'
name|'msg'
op|':'
newline|'\n'
indent|'                    '
name|'self'
op|'.'
name|'firstLineMsg'
op|'='
name|'l'
newline|'\n'
name|'break'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'                    '
name|'if'
name|'l'
op|'.'
name|'startswith'
op|'('
string|"'tree'"
op|')'
op|':'
newline|'\n'
indent|'                        '
name|'self'
op|'.'
name|'_tree'
op|'='
name|'l'
op|'['
number|'5'
op|':'
op|']'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
dedent|''
name|'elif'
name|'l'
op|'=='
string|"''"
op|':'
newline|'\n'
indent|'                        '
name|'msg'
op|'='
name|'True'
newline|'\n'
nl|'\n'
DECL|class|Graph
dedent|''
dedent|''
dedent|''
dedent|''
dedent|''
dedent|''
name|'class'
name|'Graph'
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
name|'self'
op|'.'
name|'commits'
op|'='
op|'['
op|']'
newline|'\n'
name|'self'
op|'.'
name|'shaMap'
op|'='
op|'{'
op|'}'
newline|'\n'
nl|'\n'
DECL|member|addNode
dedent|''
name|'def'
name|'addNode'
op|'('
name|'self'
op|','
name|'node'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'assert'
op|'('
name|'isinstance'
op|'('
name|'node'
op|','
name|'Commit'
op|')'
op|')'
newline|'\n'
name|'self'
op|'.'
name|'shaMap'
op|'['
name|'node'
op|'.'
name|'sha'
op|']'
op|'='
name|'node'
newline|'\n'
name|'self'
op|'.'
name|'commits'
op|'.'
name|'append'
op|'('
name|'node'
op|')'
newline|'\n'
name|'for'
name|'p'
name|'in'
name|'node'
op|'.'
name|'parents'
op|':'
newline|'\n'
indent|'            '
name|'p'
op|'.'
name|'children'
op|'.'
name|'append'
op|'('
name|'node'
op|')'
newline|'\n'
dedent|''
name|'return'
name|'node'
newline|'\n'
nl|'\n'
DECL|member|reachableNodes
dedent|''
name|'def'
name|'reachableNodes'
op|'('
name|'self'
op|','
name|'n1'
op|','
name|'n2'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'res'
op|'='
op|'{'
op|'}'
newline|'\n'
DECL|function|traverse
name|'def'
name|'traverse'
op|'('
name|'n'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'res'
op|'['
name|'n'
op|']'
op|'='
name|'True'
newline|'\n'
name|'for'
name|'p'
name|'in'
name|'n'
op|'.'
name|'parents'
op|':'
newline|'\n'
indent|'                '
name|'traverse'
op|'('
name|'p'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'traverse'
op|'('
name|'n1'
op|')'
newline|'\n'
name|'traverse'
op|'('
name|'n2'
op|')'
newline|'\n'
name|'return'
name|'res'
newline|'\n'
nl|'\n'
DECL|member|fixParents
dedent|''
name|'def'
name|'fixParents'
op|'('
name|'self'
op|','
name|'node'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'for'
name|'x'
name|'in'
name|'range'
op|'('
number|'0'
op|','
name|'len'
op|'('
name|'node'
op|'.'
name|'parents'
op|')'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'node'
op|'.'
name|'parents'
op|'['
name|'x'
op|']'
op|'='
name|'self'
op|'.'
name|'shaMap'
op|'['
name|'node'
op|'.'
name|'parents'
op|'['
name|'x'
op|']'
op|']'
newline|'\n'
nl|'\n'
comment|"# addDebug('buildGraph')"
nl|'\n'
DECL|function|buildGraph
dedent|''
dedent|''
dedent|''
name|'def'
name|'buildGraph'
op|'('
name|'heads'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'debug'
op|'('
string|"'buildGraph heads:'"
op|','
name|'heads'
op|')'
newline|'\n'
name|'for'
name|'h'
name|'in'
name|'heads'
op|':'
newline|'\n'
indent|'        '
name|'assert'
op|'('
name|'isSha'
op|'('
name|'h'
op|')'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'g'
op|'='
name|'Graph'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'out'
op|'='
name|'runProgram'
op|'('
op|'['
string|"'git-rev-list'"
op|','
string|"'--parents'"
op|']'
op|'+'
name|'heads'
op|')'
newline|'\n'
name|'for'
name|'l'
name|'in'
name|'out'
op|'.'
name|'split'
op|'('
string|"'\\n'"
op|')'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'l'
op|'=='
string|"''"
op|':'
newline|'\n'
indent|'            '
name|'continue'
newline|'\n'
dedent|''
name|'shas'
op|'='
name|'l'
op|'.'
name|'split'
op|'('
string|"' '"
op|')'
newline|'\n'
nl|'\n'
comment|"# This is a hack, we temporarily use the 'parents' attribute"
nl|'\n'
comment|'# to contain a list of SHA1:s. They are later replaced by proper'
nl|'\n'
comment|'# Commit objects.'
nl|'\n'
name|'c'
op|'='
name|'Commit'
op|'('
name|'shas'
op|'['
number|'0'
op|']'
op|','
name|'shas'
op|'['
number|'1'
op|':'
op|']'
op|')'
newline|'\n'
nl|'\n'
name|'g'
op|'.'
name|'commits'
op|'.'
name|'append'
op|'('
name|'c'
op|')'
newline|'\n'
name|'g'
op|'.'
name|'shaMap'
op|'['
name|'c'
op|'.'
name|'sha'
op|']'
op|'='
name|'c'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'c'
name|'in'
name|'g'
op|'.'
name|'commits'
op|':'
newline|'\n'
indent|'        '
name|'g'
op|'.'
name|'fixParents'
op|'('
name|'c'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'for'
name|'c'
name|'in'
name|'g'
op|'.'
name|'commits'
op|':'
newline|'\n'
indent|'        '
name|'for'
name|'p'
name|'in'
name|'c'
op|'.'
name|'parents'
op|':'
newline|'\n'
indent|'            '
name|'p'
op|'.'
name|'children'
op|'.'
name|'append'
op|'('
name|'c'
op|')'
newline|'\n'
dedent|''
dedent|''
name|'return'
name|'g'
newline|'\n'
nl|'\n'
comment|'# Write the empty tree to the object database and return its SHA1'
nl|'\n'
DECL|function|writeEmptyTree
dedent|''
name|'def'
name|'writeEmptyTree'
op|'('
op|')'
op|':'
newline|'\n'
indent|'    '
name|'tmpIndex'
op|'='
name|'os'
op|'.'
name|'environ'
op|'.'
name|'get'
op|'('
string|"'GIT_DIR'"
op|','
string|"'.git'"
op|')'
op|'+'
string|"'/merge-tmp-index'"
newline|'\n'
DECL|function|delTmpIndex
name|'def'
name|'delTmpIndex'
op|'('
op|')'
op|':'
newline|'\n'
indent|'        '
name|'try'
op|':'
newline|'\n'
indent|'            '
name|'os'
op|'.'
name|'unlink'
op|'('
name|'tmpIndex'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'OSError'
op|':'
newline|'\n'
indent|'            '
name|'pass'
newline|'\n'
dedent|''
dedent|''
name|'delTmpIndex'
op|'('
op|')'
newline|'\n'
name|'newEnv'
op|'='
name|'os'
op|'.'
name|'environ'
op|'.'
name|'copy'
op|'('
op|')'
newline|'\n'
name|'newEnv'
op|'['
string|"'GIT_INDEX_FILE'"
op|']'
op|'='
name|'tmpIndex'
newline|'\n'
name|'res'
op|'='
name|'runProgram'
op|'('
op|'['
string|"'git-write-tree'"
op|']'
op|','
name|'env'
op|'='
name|'newEnv'
op|')'
op|'.'
name|'rstrip'
op|'('
op|')'
newline|'\n'
name|'delTmpIndex'
op|'('
op|')'
newline|'\n'
name|'return'
name|'res'
newline|'\n'
nl|'\n'
DECL|function|addCommonRoot
dedent|''
name|'def'
name|'addCommonRoot'
op|'('
name|'graph'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'roots'
op|'='
op|'['
op|']'
newline|'\n'
name|'for'
name|'c'
name|'in'
name|'graph'
op|'.'
name|'commits'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'len'
op|'('
name|'c'
op|'.'
name|'parents'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'            '
name|'roots'
op|'.'
name|'append'
op|'('
name|'c'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'superRoot'
op|'='
name|'Commit'
op|'('
name|'sha'
op|'='
name|'None'
op|','
name|'parents'
op|'='
op|'['
op|']'
op|','
name|'tree'
op|'='
name|'writeEmptyTree'
op|'('
op|')'
op|')'
newline|'\n'
name|'graph'
op|'.'
name|'addNode'
op|'('
name|'superRoot'
op|')'
newline|'\n'
name|'for'
name|'r'
name|'in'
name|'roots'
op|':'
newline|'\n'
indent|'        '
name|'r'
op|'.'
name|'parents'
op|'='
op|'['
name|'superRoot'
op|']'
newline|'\n'
dedent|''
name|'superRoot'
op|'.'
name|'children'
op|'='
name|'roots'
newline|'\n'
name|'return'
name|'superRoot'
newline|'\n'
nl|'\n'
DECL|function|getCommonAncestors
dedent|''
name|'def'
name|'getCommonAncestors'
op|'('
name|'graph'
op|','
name|'commit1'
op|','
name|'commit2'
op|')'
op|':'
newline|'\n'
indent|'    '
string|"'''Find the common ancestors for commit1 and commit2'''"
newline|'\n'
name|'assert'
op|'('
name|'isinstance'
op|'('
name|'commit1'
op|','
name|'Commit'
op|')'
name|'and'
name|'isinstance'
op|'('
name|'commit2'
op|','
name|'Commit'
op|')'
op|')'
newline|'\n'
nl|'\n'
DECL|function|traverse
name|'def'
name|'traverse'
op|'('
name|'start'
op|','
name|'set'
op|')'
op|':'
newline|'\n'
indent|'        '
name|'stack'
op|'='
op|'['
name|'start'
op|']'
newline|'\n'
name|'while'
name|'len'
op|'('
name|'stack'
op|')'
op|'>'
number|'0'
op|':'
newline|'\n'
indent|'            '
name|'el'
op|'='
name|'stack'
op|'.'
name|'pop'
op|'('
op|')'
newline|'\n'
name|'set'
op|'.'
name|'add'
op|'('
name|'el'
op|')'
newline|'\n'
name|'for'
name|'p'
name|'in'
name|'el'
op|'.'
name|'parents'
op|':'
newline|'\n'
indent|'                '
name|'if'
name|'p'
name|'not'
name|'in'
name|'set'
op|':'
newline|'\n'
indent|'                    '
name|'stack'
op|'.'
name|'append'
op|'('
name|'p'
op|')'
newline|'\n'
dedent|''
dedent|''
dedent|''
dedent|''
name|'h1Set'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
name|'h2Set'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
name|'traverse'
op|'('
name|'commit1'
op|','
name|'h1Set'
op|')'
newline|'\n'
name|'traverse'
op|'('
name|'commit2'
op|','
name|'h2Set'
op|')'
newline|'\n'
name|'shared'
op|'='
name|'h1Set'
op|'.'
name|'intersection'
op|'('
name|'h2Set'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'shared'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'        '
name|'shared'
op|'='
op|'['
name|'addCommonRoot'
op|'('
name|'graph'
op|')'
op|']'
newline|'\n'
nl|'\n'
dedent|''
name|'res'
op|'='
name|'Set'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'for'
name|'s'
name|'in'
name|'shared'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'len'
op|'('
op|'['
name|'c'
name|'for'
name|'c'
name|'in'
name|'s'
op|'.'
name|'children'
name|'if'
name|'c'
name|'in'
name|'shared'
op|']'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
indent|'            '
name|'res'
op|'.'
name|'add'
op|'('
name|'s'
op|')'
newline|'\n'
dedent|''
dedent|''
name|'return'
name|'list'
op|'('
name|'res'
op|')'
newline|'\n'
dedent|''
endmarker|''
end_unit
