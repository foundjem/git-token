begin_unit
comment|'#!/usr/bin/env python'
nl|'\n'
comment|'# Copyright (c) 2010 Eric S. Raymond <esr@thyrsus.com>'
nl|'\n'
comment|'# Distributed under BSD terms.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# This script contains porcelain and porcelain byproducts.'
nl|'\n'
comment|"# It's Python because the Python standard libraries avoid portability/security"
nl|'\n'
comment|'# issues raised by callouts in the ancestral Perl and sh scripts.  It should'
nl|'\n'
comment|'# be compatible back to Python 2.1.5'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# usage: ciabot.py [-V] [-n] [-p projectname]  [refname [commits...]]'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# This script is meant to be run either in a post-commit hook or in an'
nl|'\n'
comment|'# update hook. Try it with -n to see the notification mail dumped to'
nl|'\n'
comment|'# stdout and verify that it looks sane. With -V it dumps its version'
nl|'\n'
comment|'# and exits.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# In post-commit, run it without arguments. It will query for'
nl|'\n'
comment|'# current HEAD and the latest commit ID to get the information it'
nl|'\n'
comment|'# needs.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# In update, call it with a refname followed by a list of commits:'
nl|'\n'
comment|'# You want to reverse the order git rev-list emits because it lists'
nl|'\n'
comment|'# from most recent to oldest.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# /path/to/ciabot.py ${refname} $(git rev-list ${oldhead}..${newhead} | tac)'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# Configuration variables affecting this script:'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# ciabot.project = name of the project'
nl|'\n'
comment|'# ciabot.repo = name of the project repo for gitweb/cgit purposes'
nl|'\n'
comment|'# ciabot.xmlrpc  = if true (default), ship notifications via XML-RPC'
nl|'\n'
comment|'# ciabot.revformat = format in which the revision is shown'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# ciabot.project defaults to the directory name of the repository toplevel.'
nl|'\n'
comment|'# ciabot.repo defaults to ciabot.project lowercased.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# This means that in the normal case you need not do any configuration at all,'
nl|'\n'
comment|'# but setting the project name will speed it up slightly.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# The revformat variable may have the following values'
nl|'\n'
comment|'# raw -> full hex ID of commit'
nl|'\n'
comment|'# short -> first 12 chars of hex ID'
nl|'\n'
comment|'# describe = -> describe relative to last tag, falling back to short'
nl|'\n'
comment|"# The default is 'describe'."
nl|'\n'
comment|'#'
nl|'\n'
comment|'# Note: the CIA project now says only XML-RPC is reliable, so'
nl|'\n'
comment|'# we default to that.'
nl|'\n'
comment|'#'
nl|'\n'
nl|'\n'
name|'import'
name|'sys'
newline|'\n'
name|'if'
name|'sys'
op|'.'
name|'hexversion'
op|'<'
number|'0x02000000'
op|':'
newline|'\n'
comment|'# The limiter is the xml.sax module'
nl|'\n'
indent|'        '
name|'sys'
op|'.'
name|'stderr'
op|'.'
name|'write'
op|'('
string|'"ciabot.py: requires Python 2.0.0 or later.\\n"'
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
name|'import'
name|'os'
op|','
name|'commands'
op|','
name|'socket'
op|','
name|'urllib'
newline|'\n'
name|'from'
name|'xml'
op|'.'
name|'sax'
op|'.'
name|'saxutils'
name|'import'
name|'escape'
newline|'\n'
nl|'\n'
comment|'# Changeset URL prefix for your repo: when the commit ID is appended'
nl|'\n'
comment|'# to this, it should point at a CGI that will display the commit'
nl|'\n'
comment|'# through gitweb or something similar. The defaults will probably'
nl|'\n'
comment|'# work if you have a typical gitweb/cgit setup.'
nl|'\n'
comment|'#'
nl|'\n'
comment|'#urlprefix="http://%(host)s/cgi-bin/gitweb.cgi?p=%(repo)s;a=commit;h="'
nl|'\n'
DECL|variable|urlprefix
name|'urlprefix'
op|'='
string|'"http://%(host)s/cgi-bin/cgit.cgi/%(repo)s/commit/?id="'
newline|'\n'
nl|'\n'
comment|'# The service used to turn your gitwebbish URL into a tinyurl so it'
nl|'\n'
comment|'# will take up less space on the IRC notification line.'
nl|'\n'
name|'tinyifier'
op|'='
string|'"http://tinyurl.com/api-create.php?url="'
newline|'\n'
nl|'\n'
comment|'# The template used to generate the XML messages to CIA.  You can make'
nl|'\n'
comment|'# visible changes to the IRC-bot notification lines by hacking this.'
nl|'\n'
comment|'# The default will produce a notfication line that looks like this:'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# ${project}: ${author} ${repo}:${branch} * ${rev} ${files}: ${logmsg} ${url}'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# By omitting $files you can collapse the files part to a single slash.'
nl|'\n'
name|'xml'
op|'='
string|"'''\\\n<message>\n  <generator>\n    <name>CIA Python client for Git</name>\n    <version>%(version)s</version>\n    <url>%(generator)s</url>\n  </generator>\n  <source>\n    <project>%(project)s</project>\n    <branch>%(repo)s:%(branch)s</branch>\n  </source>\n  <timestamp>%(ts)s</timestamp>\n  <body>\n    <commit>\n      <author>%(author)s</author>\n      <revision>%(rev)s</revision>\n      <files>\n        %(files)s\n      </files>\n      <log>%(logmsg)s %(url)s</log>\n      <url>%(url)s</url>\n    </commit>\n  </body>\n</message>\n'''"
newline|'\n'
nl|'\n'
comment|'#'
nl|'\n'
comment|'# No user-serviceable parts below this line:'
nl|'\n'
comment|'#'
nl|'\n'
nl|'\n'
comment|'# Where to ship e-mail notifications.'
nl|'\n'
DECL|variable|toaddr
name|'toaddr'
op|'='
string|'"cia@cia.vc"'
newline|'\n'
nl|'\n'
comment|'# Identify the generator script.'
nl|'\n'
comment|'# Should only change when the script itself gets a new home and maintainer.'
nl|'\n'
DECL|variable|generator
name|'generator'
op|'='
string|'"http://www.catb.org/~esr/ciabot.py"'
newline|'\n'
DECL|variable|version
name|'version'
op|'='
string|'"3.6"'
newline|'\n'
nl|'\n'
DECL|function|do
name|'def'
name|'do'
op|'('
name|'command'
op|')'
op|':'
newline|'\n'
indent|'    '
name|'return'
name|'commands'
op|'.'
name|'getstatusoutput'
op|'('
name|'command'
op|')'
op|'['
number|'1'
op|']'
newline|'\n'
nl|'\n'
DECL|function|report
dedent|''
name|'def'
name|'report'
op|'('
name|'refname'
op|','
name|'merged'
op|','
name|'xmlrpc'
op|'='
name|'True'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"Generate a commit notification to be reported to CIA"'
newline|'\n'
nl|'\n'
comment|'# Try to tinyfy a reference to a web view for this commit.'
nl|'\n'
name|'try'
op|':'
newline|'\n'
indent|'        '
name|'url'
op|'='
name|'open'
op|'('
name|'urllib'
op|'.'
name|'urlretrieve'
op|'('
name|'tinyifier'
op|'+'
name|'urlprefix'
op|'+'
name|'merged'
op|')'
op|'['
number|'0'
op|']'
op|')'
op|'.'
name|'read'
op|'('
op|')'
newline|'\n'
dedent|''
name|'except'
op|':'
newline|'\n'
indent|'        '
name|'url'
op|'='
name|'urlprefix'
op|'+'
name|'merged'
newline|'\n'
nl|'\n'
dedent|''
name|'branch'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'basename'
op|'('
name|'refname'
op|')'
newline|'\n'
nl|'\n'
comment|'# Compute a description for the revision'
nl|'\n'
name|'if'
name|'revformat'
op|'=='
string|"'raw'"
op|':'
newline|'\n'
indent|'        '
name|'rev'
op|'='
name|'merged'
newline|'\n'
dedent|''
name|'elif'
name|'revformat'
op|'=='
string|"'short'"
op|':'
newline|'\n'
indent|'        '
name|'rev'
op|'='
string|"''"
newline|'\n'
dedent|''
name|'else'
op|':'
comment|"# revformat == 'describe'"
newline|'\n'
indent|'        '
name|'rev'
op|'='
name|'do'
op|'('
string|'"git describe %s 2>/dev/null"'
op|'%'
name|'merged'
op|')'
newline|'\n'
dedent|''
name|'if'
name|'not'
name|'rev'
op|':'
newline|'\n'
indent|'        '
name|'rev'
op|'='
name|'merged'
op|'['
op|':'
number|'12'
op|']'
newline|'\n'
nl|'\n'
comment|'# Extract the meta-information for the commit'
nl|'\n'
dedent|''
name|'files'
op|'='
name|'do'
op|'('
string|'"git diff-tree -r --name-only \'"'
op|'+'
name|'merged'
op|'+'
string|'"\' | sed -e \'1d\' -e \'s-.*-<file>&</file>-\'"'
op|')'
newline|'\n'
name|'metainfo'
op|'='
name|'do'
op|'('
string|'"git log -1 \'--pretty=format:%an <%ae>%n%at%n%s\' "'
op|'+'
name|'merged'
op|')'
newline|'\n'
op|'('
name|'author'
op|','
name|'ts'
op|','
name|'logmsg'
op|')'
op|'='
name|'metainfo'
op|'.'
name|'split'
op|'('
string|'"\\n"'
op|')'
newline|'\n'
name|'logmsg'
op|'='
name|'escape'
op|'('
name|'logmsg'
op|')'
newline|'\n'
nl|'\n'
comment|"# This discards the part of the author's address after @."
nl|'\n'
comment|'# Might be be nice to ship the full email address, if not'
nl|'\n'
comment|"# for spammers' address harvesters - getting this wrong"
nl|'\n'
comment|'# would make the freenode #commits channel into harvester heaven.'
nl|'\n'
name|'author'
op|'='
name|'escape'
op|'('
name|'author'
op|'.'
name|'replace'
op|'('
string|'"<"'
op|','
string|'""'
op|')'
op|'.'
name|'split'
op|'('
string|'"@"'
op|')'
op|'['
number|'0'
op|']'
op|'.'
name|'split'
op|'('
op|')'
op|'['
op|'-'
number|'1'
op|']'
op|')'
newline|'\n'
nl|'\n'
comment|'# This ignores the timezone.  Not clear what to do with it...'
nl|'\n'
name|'ts'
op|'='
name|'ts'
op|'.'
name|'strip'
op|'('
op|')'
op|'.'
name|'split'
op|'('
op|')'
op|'['
number|'0'
op|']'
newline|'\n'
nl|'\n'
name|'context'
op|'='
name|'locals'
op|'('
op|')'
newline|'\n'
name|'context'
op|'.'
name|'update'
op|'('
name|'globals'
op|'('
op|')'
op|')'
newline|'\n'
nl|'\n'
name|'out'
op|'='
name|'xml'
op|'%'
name|'context'
newline|'\n'
name|'mail'
op|'='
string|"'''\\\nMessage-ID: <%(merged)s.%(author)s@%(project)s>\nFrom: %(fromaddr)s\nTo: %(toaddr)s\nContent-type: text/xml\nSubject: DeliverXML\n\n%(out)s'''"
op|'%'
name|'locals'
op|'('
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'xmlrpc'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'out'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'        '
name|'return'
name|'mail'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'__name__'
op|'=='
string|'"__main__"'
op|':'
newline|'\n'
indent|'    '
name|'import'
name|'getopt'
newline|'\n'
nl|'\n'
comment|'# Get all config variables'
nl|'\n'
DECL|variable|revformat
name|'revformat'
op|'='
name|'do'
op|'('
string|'"git config --get ciabot.revformat"'
op|')'
newline|'\n'
DECL|variable|project
name|'project'
op|'='
name|'do'
op|'('
string|'"git config --get ciabot.project"'
op|')'
newline|'\n'
DECL|variable|repo
name|'repo'
op|'='
name|'do'
op|'('
string|'"git config --get ciabot.repo"'
op|')'
newline|'\n'
DECL|variable|xmlrpc
name|'xmlrpc'
op|'='
name|'do'
op|'('
string|'"git config --get ciabot.xmlrpc"'
op|')'
newline|'\n'
DECL|variable|xmlrpc
name|'xmlrpc'
op|'='
name|'not'
op|'('
name|'xmlrpc'
name|'and'
name|'xmlrpc'
op|'=='
string|'"false"'
op|')'
newline|'\n'
nl|'\n'
DECL|variable|host
name|'host'
op|'='
name|'socket'
op|'.'
name|'getfqdn'
op|'('
op|')'
newline|'\n'
DECL|variable|fromaddr
name|'fromaddr'
op|'='
string|'"CIABOT-NOREPLY@"'
op|'+'
name|'host'
newline|'\n'
nl|'\n'
name|'try'
op|':'
newline|'\n'
indent|'        '
op|'('
name|'options'
op|','
name|'arguments'
op|')'
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
string|'"np:xV"'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'getopt'
op|'.'
name|'GetoptError'
op|','
name|'msg'
op|':'
newline|'\n'
indent|'        '
name|'print'
string|'"ciabot.py: "'
op|'+'
name|'str'
op|'('
name|'msg'
op|')'
newline|'\n'
name|'raise'
name|'SystemExit'
op|','
number|'1'
newline|'\n'
nl|'\n'
DECL|variable|notify
dedent|''
name|'notify'
op|'='
name|'True'
newline|'\n'
name|'for'
op|'('
name|'switch'
op|','
name|'val'
op|')'
name|'in'
name|'options'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'switch'
op|'=='
string|"'-p'"
op|':'
newline|'\n'
DECL|variable|project
indent|'            '
name|'project'
op|'='
name|'val'
newline|'\n'
dedent|''
name|'elif'
name|'switch'
op|'=='
string|"'-n'"
op|':'
newline|'\n'
DECL|variable|notify
indent|'            '
name|'notify'
op|'='
name|'False'
newline|'\n'
dedent|''
name|'elif'
name|'switch'
op|'=='
string|"'-x'"
op|':'
newline|'\n'
DECL|variable|xmlrpc
indent|'            '
name|'xmlrpc'
op|'='
name|'True'
newline|'\n'
dedent|''
name|'elif'
name|'switch'
op|'=='
string|"'-V'"
op|':'
newline|'\n'
indent|'            '
name|'print'
string|'"ciabot.py: version"'
op|','
name|'version'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'0'
op|')'
newline|'\n'
nl|'\n'
comment|'# The project variable defaults to the name of the repository toplevel.'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'not'
name|'project'
op|':'
newline|'\n'
DECL|variable|here
indent|'        '
name|'here'
op|'='
name|'os'
op|'.'
name|'getcwd'
op|'('
op|')'
newline|'\n'
name|'while'
name|'True'
op|':'
newline|'\n'
indent|'            '
name|'if'
name|'os'
op|'.'
name|'path'
op|'.'
name|'exists'
op|'('
name|'os'
op|'.'
name|'path'
op|'.'
name|'join'
op|'('
name|'here'
op|','
string|'".git"'
op|')'
op|')'
op|':'
newline|'\n'
DECL|variable|project
indent|'                '
name|'project'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'basename'
op|'('
name|'here'
op|')'
newline|'\n'
name|'break'
newline|'\n'
dedent|''
name|'elif'
name|'here'
op|'=='
string|"'/'"
op|':'
newline|'\n'
indent|'                '
name|'sys'
op|'.'
name|'stderr'
op|'.'
name|'write'
op|'('
string|'"ciabot.py: no .git below root!\\n"'
op|')'
newline|'\n'
name|'sys'
op|'.'
name|'exit'
op|'('
number|'1'
op|')'
newline|'\n'
DECL|variable|here
dedent|''
name|'here'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'dirname'
op|'('
name|'here'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'not'
name|'repo'
op|':'
newline|'\n'
DECL|variable|repo
indent|'        '
name|'repo'
op|'='
name|'project'
op|'.'
name|'lower'
op|'('
op|')'
newline|'\n'
nl|'\n'
DECL|variable|urlprefix
dedent|''
name|'urlprefix'
op|'='
name|'urlprefix'
op|'%'
name|'globals'
op|'('
op|')'
newline|'\n'
nl|'\n'
comment|'# The script wants a reference to head followed by the list of'
nl|'\n'
comment|'# commit ID to report about.'
nl|'\n'
name|'if'
name|'len'
op|'('
name|'arguments'
op|')'
op|'=='
number|'0'
op|':'
newline|'\n'
DECL|variable|refname
indent|'        '
name|'refname'
op|'='
name|'do'
op|'('
string|'"git symbolic-ref HEAD 2>/dev/null"'
op|')'
newline|'\n'
DECL|variable|merges
name|'merges'
op|'='
op|'['
name|'do'
op|'('
string|'"git rev-parse HEAD"'
op|')'
op|']'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
DECL|variable|refname
indent|'        '
name|'refname'
op|'='
name|'arguments'
op|'['
number|'0'
op|']'
newline|'\n'
DECL|variable|merges
name|'merges'
op|'='
name|'arguments'
op|'['
number|'1'
op|':'
op|']'
newline|'\n'
nl|'\n'
dedent|''
name|'if'
name|'notify'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'xmlrpc'
op|':'
newline|'\n'
indent|'            '
name|'import'
name|'xmlrpclib'
newline|'\n'
DECL|variable|server
name|'server'
op|'='
name|'xmlrpclib'
op|'.'
name|'Server'
op|'('
string|"'http://cia.vc/RPC2'"
op|')'
op|';'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'import'
name|'smtplib'
newline|'\n'
DECL|variable|server
name|'server'
op|'='
name|'smtplib'
op|'.'
name|'SMTP'
op|'('
string|"'localhost'"
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'for'
name|'merged'
name|'in'
name|'merges'
op|':'
newline|'\n'
DECL|variable|message
indent|'        '
name|'message'
op|'='
name|'report'
op|'('
name|'refname'
op|','
name|'merged'
op|','
name|'xmlrpc'
op|')'
newline|'\n'
name|'if'
name|'not'
name|'notify'
op|':'
newline|'\n'
indent|'            '
name|'print'
name|'message'
newline|'\n'
dedent|''
name|'elif'
name|'xmlrpc'
op|':'
newline|'\n'
indent|'            '
name|'try'
op|':'
newline|'\n'
comment|'# RPC server is flaky, this can fail due to timeout.'
nl|'\n'
indent|'                '
name|'server'
op|'.'
name|'hub'
op|'.'
name|'deliver'
op|'('
name|'message'
op|')'
newline|'\n'
dedent|''
name|'except'
name|'socket'
op|'.'
name|'error'
op|','
name|'e'
op|':'
newline|'\n'
indent|'                '
name|'sys'
op|'.'
name|'stderr'
op|'.'
name|'write'
op|'('
string|'"%s\\n"'
op|'%'
name|'e'
op|')'
newline|'\n'
dedent|''
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'server'
op|'.'
name|'sendmail'
op|'('
name|'fromaddr'
op|','
op|'['
name|'toaddr'
op|']'
op|','
name|'message'
op|')'
newline|'\n'
nl|'\n'
dedent|''
dedent|''
name|'if'
name|'notify'
op|':'
newline|'\n'
indent|'        '
name|'if'
name|'not'
name|'xmlrpc'
op|':'
newline|'\n'
indent|'            '
name|'server'
op|'.'
name|'quit'
op|'('
op|')'
newline|'\n'
nl|'\n'
comment|'#End'
nl|'\n'
dedent|''
dedent|''
dedent|''
endmarker|''
end_unit
