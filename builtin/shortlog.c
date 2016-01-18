begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_include
include|#
directive|include
file|"mailmap.h"
end_include
begin_include
include|#
directive|include
file|"shortlog.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|shortlog_usage
specifier|static
name|char
specifier|const
modifier|*
specifier|const
name|shortlog_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git shortlog [<options>] [<revision-range>] [[--] [<path>...]]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * The util field of our string_list_items will contain one of two things:  *  *   - if --summary is not in use, it will point to a string list of the  *     oneline subjects assigned to this author  *  *   - if --summary is in use, we don't need that list; we only need to know  *     its size. So we abuse the pointer slot to store our integer counter.  *  *  This macro accesses the latter.  */
end_comment
begin_define
DECL|macro|UTIL_TO_INT
define|#
directive|define
name|UTIL_TO_INT
parameter_list|(
name|x
parameter_list|)
value|((intptr_t)(x)->util)
end_define
begin_function
DECL|function|compare_by_counter
specifier|static
name|int
name|compare_by_counter
parameter_list|(
specifier|const
name|void
modifier|*
name|a1
parameter_list|,
specifier|const
name|void
modifier|*
name|a2
parameter_list|)
block|{
specifier|const
name|struct
name|string_list_item
modifier|*
name|i1
init|=
name|a1
decl_stmt|,
modifier|*
name|i2
init|=
name|a2
decl_stmt|;
return|return
name|UTIL_TO_INT
argument_list|(
name|i2
argument_list|)
operator|-
name|UTIL_TO_INT
argument_list|(
name|i1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|compare_by_list
specifier|static
name|int
name|compare_by_list
parameter_list|(
specifier|const
name|void
modifier|*
name|a1
parameter_list|,
specifier|const
name|void
modifier|*
name|a2
parameter_list|)
block|{
specifier|const
name|struct
name|string_list_item
modifier|*
name|i1
init|=
name|a1
decl_stmt|,
modifier|*
name|i2
init|=
name|a2
decl_stmt|;
specifier|const
name|struct
name|string_list
modifier|*
name|l1
init|=
name|i1
operator|->
name|util
decl_stmt|,
modifier|*
name|l2
init|=
name|i2
operator|->
name|util
decl_stmt|;
if|if
condition|(
name|l1
operator|->
name|nr
operator|<
name|l2
operator|->
name|nr
condition|)
return|return
literal|1
return|;
elseif|else
if|if
condition|(
name|l1
operator|->
name|nr
operator|==
name|l2
operator|->
name|nr
condition|)
return|return
literal|0
return|;
else|else
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|insert_one_record
specifier|static
name|void
name|insert_one_record
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
specifier|const
name|char
modifier|*
name|oneline
parameter_list|)
block|{
name|struct
name|string_list_item
modifier|*
name|item
decl_stmt|;
specifier|const
name|char
modifier|*
name|mailbuf
decl_stmt|,
modifier|*
name|namebuf
decl_stmt|;
name|size_t
name|namelen
decl_stmt|,
name|maillen
decl_stmt|;
name|struct
name|strbuf
name|namemailbuf
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|ident_split
name|ident
decl_stmt|;
if|if
condition|(
name|split_ident_line
argument_list|(
operator|&
name|ident
argument_list|,
name|author
argument_list|,
name|strlen
argument_list|(
name|author
argument_list|)
argument_list|)
condition|)
return|return;
name|namebuf
operator|=
name|ident
operator|.
name|name_begin
expr_stmt|;
name|mailbuf
operator|=
name|ident
operator|.
name|mail_begin
expr_stmt|;
name|namelen
operator|=
name|ident
operator|.
name|name_end
operator|-
name|ident
operator|.
name|name_begin
expr_stmt|;
name|maillen
operator|=
name|ident
operator|.
name|mail_end
operator|-
name|ident
operator|.
name|mail_begin
expr_stmt|;
name|map_user
argument_list|(
operator|&
name|log
operator|->
name|mailmap
argument_list|,
operator|&
name|mailbuf
argument_list|,
operator|&
name|maillen
argument_list|,
operator|&
name|namebuf
argument_list|,
operator|&
name|namelen
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|namemailbuf
argument_list|,
name|namebuf
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
name|log
operator|->
name|email
condition|)
name|strbuf_addf
argument_list|(
operator|&
name|namemailbuf
argument_list|,
literal|"<%.*s>"
argument_list|,
operator|(
name|int
operator|)
name|maillen
argument_list|,
name|mailbuf
argument_list|)
expr_stmt|;
name|item
operator|=
name|string_list_insert
argument_list|(
operator|&
name|log
operator|->
name|list
argument_list|,
name|namemailbuf
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|log
operator|->
name|summary
condition|)
name|item
operator|->
name|util
operator|=
operator|(
name|void
operator|*
operator|)
operator|(
name|UTIL_TO_INT
argument_list|(
name|item
argument_list|)
operator|+
literal|1
operator|)
expr_stmt|;
else|else
block|{
specifier|const
name|char
modifier|*
name|dot3
init|=
name|log
operator|->
name|common_repo_prefix
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|struct
name|strbuf
name|subject
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|eol
decl_stmt|;
comment|/* Skip any leading whitespace, including any blank lines. */
while|while
condition|(
operator|*
name|oneline
operator|&&
name|isspace
argument_list|(
operator|*
name|oneline
argument_list|)
condition|)
name|oneline
operator|++
expr_stmt|;
name|eol
operator|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eol
condition|)
name|eol
operator|=
name|oneline
operator|+
name|strlen
argument_list|(
name|oneline
argument_list|)
expr_stmt|;
if|if
condition|(
name|starts_with
argument_list|(
name|oneline
argument_list|,
literal|"[PATCH"
argument_list|)
condition|)
block|{
name|char
modifier|*
name|eob
init|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|']'
argument_list|)
decl_stmt|;
if|if
condition|(
name|eob
operator|&&
operator|(
operator|!
name|eol
operator|||
name|eob
operator|<
name|eol
operator|)
condition|)
name|oneline
operator|=
name|eob
operator|+
literal|1
expr_stmt|;
block|}
while|while
condition|(
operator|*
name|oneline
operator|&&
name|isspace
argument_list|(
operator|*
name|oneline
argument_list|)
operator|&&
operator|*
name|oneline
operator|!=
literal|'\n'
condition|)
name|oneline
operator|++
expr_stmt|;
name|format_subject
argument_list|(
operator|&
name|subject
argument_list|,
name|oneline
argument_list|,
literal|" "
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|subject
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|dot3
condition|)
block|{
name|int
name|dot3len
init|=
name|strlen
argument_list|(
name|dot3
argument_list|)
decl_stmt|;
if|if
condition|(
name|dot3len
operator|>
literal|5
condition|)
block|{
while|while
condition|(
operator|(
name|p
operator|=
name|strstr
argument_list|(
name|buffer
argument_list|,
name|dot3
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|taillen
init|=
name|strlen
argument_list|(
name|p
argument_list|)
operator|-
name|dot3len
decl_stmt|;
name|memcpy
argument_list|(
name|p
argument_list|,
literal|"/.../"
argument_list|,
literal|5
argument_list|)
expr_stmt|;
name|memmove
argument_list|(
name|p
operator|+
literal|5
argument_list|,
name|p
operator|+
name|dot3len
argument_list|,
name|taillen
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
block|}
if|if
condition|(
name|item
operator|->
name|util
operator|==
name|NULL
condition|)
name|item
operator|->
name|util
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|string_list
argument_list|)
argument_list|)
expr_stmt|;
name|string_list_append
argument_list|(
name|item
operator|->
name|util
argument_list|,
name|buffer
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|read_from_stdin
specifier|static
name|void
name|read_from_stdin
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|struct
name|strbuf
name|author
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|oneline
init|=
name|STRBUF_INIT
decl_stmt|;
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|author
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
condition|)
block|{
specifier|const
name|char
modifier|*
name|v
decl_stmt|;
if|if
condition|(
operator|!
name|skip_prefix
argument_list|(
name|author
operator|.
name|buf
argument_list|,
literal|"Author: "
argument_list|,
operator|&
name|v
argument_list|)
operator|&&
operator|!
name|skip_prefix
argument_list|(
name|author
operator|.
name|buf
argument_list|,
literal|"author "
argument_list|,
operator|&
name|v
argument_list|)
condition|)
continue|continue;
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|oneline
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
operator|&&
name|oneline
operator|.
name|len
condition|)
empty_stmt|;
comment|/* discard headers */
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|oneline
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
operator|&&
operator|!
name|oneline
operator|.
name|len
condition|)
empty_stmt|;
comment|/* discard blanks */
name|insert_one_record
argument_list|(
name|log
argument_list|,
name|v
argument_list|,
name|oneline
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|author
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|oneline
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|shortlog_add_commit
name|void
name|shortlog_add_commit
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|strbuf
name|author
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|oneline
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|pretty_print_context
name|ctx
init|=
block|{
literal|0
block|}
decl_stmt|;
name|ctx
operator|.
name|fmt
operator|=
name|CMIT_FMT_USERFORMAT
expr_stmt|;
name|ctx
operator|.
name|abbrev
operator|=
name|log
operator|->
name|abbrev
expr_stmt|;
name|ctx
operator|.
name|subject
operator|=
literal|""
expr_stmt|;
name|ctx
operator|.
name|after_subject
operator|=
literal|""
expr_stmt|;
name|ctx
operator|.
name|date_mode
operator|.
name|type
operator|=
name|DATE_NORMAL
expr_stmt|;
name|ctx
operator|.
name|output_encoding
operator|=
name|get_log_output_encoding
argument_list|()
expr_stmt|;
name|format_commit_message
argument_list|(
name|commit
argument_list|,
literal|"%an<%ae>"
argument_list|,
operator|&
name|author
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
comment|/* we can detect a total failure only by seeing "<>" in the output */
if|if
condition|(
name|author
operator|.
name|len
operator|<=
literal|3
condition|)
block|{
name|warning
argument_list|(
name|_
argument_list|(
literal|"Missing author: %s"
argument_list|)
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|commit
operator|->
name|object
operator|.
name|oid
argument_list|)
argument_list|)
expr_stmt|;
goto|goto
name|out
goto|;
block|}
if|if
condition|(
operator|!
name|log
operator|->
name|summary
condition|)
block|{
if|if
condition|(
name|log
operator|->
name|user_format
condition|)
name|pretty_print_commit
argument_list|(
operator|&
name|ctx
argument_list|,
name|commit
argument_list|,
operator|&
name|oneline
argument_list|)
expr_stmt|;
else|else
name|format_commit_message
argument_list|(
name|commit
argument_list|,
literal|"%s"
argument_list|,
operator|&
name|oneline
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
block|}
name|insert_one_record
argument_list|(
name|log
argument_list|,
name|author
operator|.
name|buf
argument_list|,
name|oneline
operator|.
name|len
condition|?
name|oneline
operator|.
name|buf
else|:
literal|"<none>"
argument_list|)
expr_stmt|;
name|out
label|:
name|strbuf_release
argument_list|(
operator|&
name|author
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|oneline
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_from_rev
specifier|static
name|void
name|get_from_rev
parameter_list|(
name|struct
name|rev_info
modifier|*
name|rev
parameter_list|,
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
name|prepare_revision_walk
argument_list|(
name|rev
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"revision walk setup failed"
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
name|rev
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
name|shortlog_add_commit
argument_list|(
name|log
argument_list|,
name|commit
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_uint
specifier|static
name|int
name|parse_uint
parameter_list|(
name|char
specifier|const
modifier|*
modifier|*
name|arg
parameter_list|,
name|int
name|comma
parameter_list|,
name|int
name|defval
parameter_list|)
block|{
name|unsigned
name|long
name|ul
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|char
modifier|*
name|endp
decl_stmt|;
name|ul
operator|=
name|strtoul
argument_list|(
operator|*
name|arg
argument_list|,
operator|&
name|endp
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|endp
operator|&&
operator|*
name|endp
operator|!=
name|comma
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|ul
operator|>
name|INT_MAX
condition|)
return|return
operator|-
literal|1
return|;
name|ret
operator|=
operator|*
name|arg
operator|==
name|endp
condition|?
name|defval
else|:
operator|(
name|int
operator|)
name|ul
expr_stmt|;
operator|*
name|arg
operator|=
operator|*
name|endp
condition|?
name|endp
operator|+
literal|1
else|:
name|endp
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|wrap_arg_usage
specifier|static
specifier|const
name|char
name|wrap_arg_usage
index|[]
init|=
literal|"-w[<width>[,<indent1>[,<indent2>]]]"
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|DEFAULT_WRAPLEN
define|#
directive|define
name|DEFAULT_WRAPLEN
value|76
end_define
begin_define
DECL|macro|DEFAULT_INDENT1
define|#
directive|define
name|DEFAULT_INDENT1
value|6
end_define
begin_define
DECL|macro|DEFAULT_INDENT2
define|#
directive|define
name|DEFAULT_INDENT2
value|9
end_define
begin_function
DECL|function|parse_wrap_args
specifier|static
name|int
name|parse_wrap_args
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|shortlog
modifier|*
name|log
init|=
name|opt
operator|->
name|value
decl_stmt|;
name|log
operator|->
name|wrap_lines
operator|=
operator|!
name|unset
expr_stmt|;
if|if
condition|(
name|unset
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|arg
condition|)
block|{
name|log
operator|->
name|wrap
operator|=
name|DEFAULT_WRAPLEN
expr_stmt|;
name|log
operator|->
name|in1
operator|=
name|DEFAULT_INDENT1
expr_stmt|;
name|log
operator|->
name|in2
operator|=
name|DEFAULT_INDENT2
expr_stmt|;
return|return
literal|0
return|;
block|}
name|log
operator|->
name|wrap
operator|=
name|parse_uint
argument_list|(
operator|&
name|arg
argument_list|,
literal|','
argument_list|,
name|DEFAULT_WRAPLEN
argument_list|)
expr_stmt|;
name|log
operator|->
name|in1
operator|=
name|parse_uint
argument_list|(
operator|&
name|arg
argument_list|,
literal|','
argument_list|,
name|DEFAULT_INDENT1
argument_list|)
expr_stmt|;
name|log
operator|->
name|in2
operator|=
name|parse_uint
argument_list|(
operator|&
name|arg
argument_list|,
literal|'\0'
argument_list|,
name|DEFAULT_INDENT2
argument_list|)
expr_stmt|;
if|if
condition|(
name|log
operator|->
name|wrap
operator|<
literal|0
operator|||
name|log
operator|->
name|in1
operator|<
literal|0
operator|||
name|log
operator|->
name|in2
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
name|wrap_arg_usage
argument_list|)
return|;
if|if
condition|(
name|log
operator|->
name|wrap
operator|&&
operator|(
operator|(
name|log
operator|->
name|in1
operator|&&
name|log
operator|->
name|wrap
operator|<=
name|log
operator|->
name|in1
operator|)
operator|||
operator|(
name|log
operator|->
name|in2
operator|&&
name|log
operator|->
name|wrap
operator|<=
name|log
operator|->
name|in2
operator|)
operator|)
condition|)
return|return
name|error
argument_list|(
name|wrap_arg_usage
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|shortlog_init
name|void
name|shortlog_init
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|memset
argument_list|(
name|log
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|log
argument_list|)
argument_list|)
expr_stmt|;
name|read_mailmap
argument_list|(
operator|&
name|log
operator|->
name|mailmap
argument_list|,
operator|&
name|log
operator|->
name|common_repo_prefix
argument_list|)
expr_stmt|;
name|log
operator|->
name|list
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|log
operator|->
name|wrap
operator|=
name|DEFAULT_WRAPLEN
expr_stmt|;
name|log
operator|->
name|in1
operator|=
name|DEFAULT_INDENT1
expr_stmt|;
name|log
operator|->
name|in2
operator|=
name|DEFAULT_INDENT2
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_shortlog
name|int
name|cmd_shortlog
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
specifier|static
name|struct
name|shortlog
name|log
decl_stmt|;
specifier|static
name|struct
name|rev_info
name|rev
decl_stmt|;
name|int
name|nongit
init|=
operator|!
name|startup_info
operator|->
name|have_repository
decl_stmt|;
specifier|static
specifier|const
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOL
argument_list|(
literal|'n'
argument_list|,
literal|"numbered"
argument_list|,
operator|&
name|log
operator|.
name|sort_by_number
argument_list|,
name|N_
argument_list|(
literal|"sort output according to the number of commits per author"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'s'
argument_list|,
literal|"summary"
argument_list|,
operator|&
name|log
operator|.
name|summary
argument_list|,
name|N_
argument_list|(
literal|"Suppress commit descriptions, only provides commit count"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'e'
argument_list|,
literal|"email"
argument_list|,
operator|&
name|log
operator|.
name|email
argument_list|,
name|N_
argument_list|(
literal|"Show the email address of each author"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_CALLBACK
block|,
literal|'w'
block|,
name|NULL
block|,
operator|&
name|log
block|,
name|N_
argument_list|(
literal|"w[,i1[,i2]]"
argument_list|)
block|,
name|N_
argument_list|(
literal|"Linewrap output"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
block|,
operator|&
name|parse_wrap_args
block|}
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|struct
name|parse_opt_ctx_t
name|ctx
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|shortlog_init
argument_list|(
operator|&
name|log
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|parse_options_start
argument_list|(
operator|&
name|ctx
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|PARSE_OPT_KEEP_DASHDASH
operator||
name|PARSE_OPT_KEEP_ARGV0
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
switch|switch
condition|(
name|parse_options_step
argument_list|(
operator|&
name|ctx
argument_list|,
name|options
argument_list|,
name|shortlog_usage
argument_list|)
condition|)
block|{
case|case
name|PARSE_OPT_HELP
case|:
name|exit
argument_list|(
literal|129
argument_list|)
expr_stmt|;
case|case
name|PARSE_OPT_DONE
case|:
goto|goto
name|parse_done
goto|;
block|}
name|parse_revision_opt
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|ctx
argument_list|,
name|options
argument_list|,
name|shortlog_usage
argument_list|)
expr_stmt|;
block|}
name|parse_done
label|:
name|argc
operator|=
name|parse_options_end
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
if|if
condition|(
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
operator|!=
literal|1
condition|)
block|{
name|error
argument_list|(
name|_
argument_list|(
literal|"unrecognized argument: %s"
argument_list|)
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|shortlog_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
name|log
operator|.
name|user_format
operator|=
name|rev
operator|.
name|commit_format
operator|==
name|CMIT_FMT_USERFORMAT
expr_stmt|;
name|log
operator|.
name|abbrev
operator|=
name|rev
operator|.
name|abbrev
expr_stmt|;
comment|/* assume HEAD if from a tty */
if|if
condition|(
operator|!
name|nongit
operator|&&
operator|!
name|rev
operator|.
name|pending
operator|.
name|nr
operator|&&
name|isatty
argument_list|(
literal|0
argument_list|)
condition|)
name|add_head_to_pending
argument_list|(
operator|&
name|rev
argument_list|)
expr_stmt|;
if|if
condition|(
name|rev
operator|.
name|pending
operator|.
name|nr
operator|==
literal|0
condition|)
block|{
if|if
condition|(
name|isatty
argument_list|(
literal|0
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"(reading log message from standard input)\n"
argument_list|)
argument_list|)
expr_stmt|;
name|read_from_stdin
argument_list|(
operator|&
name|log
argument_list|)
expr_stmt|;
block|}
else|else
name|get_from_rev
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|log
argument_list|)
expr_stmt|;
name|shortlog_output
argument_list|(
operator|&
name|log
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_wrapped_shortlog_msg
specifier|static
name|void
name|add_wrapped_shortlog_msg
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|,
specifier|const
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|strbuf_add_wrapped_text
argument_list|(
name|sb
argument_list|,
name|s
argument_list|,
name|log
operator|->
name|in1
argument_list|,
name|log
operator|->
name|in2
argument_list|,
name|log
operator|->
name|wrap
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|shortlog_output
name|void
name|shortlog_output
parameter_list|(
name|struct
name|shortlog
modifier|*
name|log
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|log
operator|->
name|sort_by_number
condition|)
name|qsort
argument_list|(
name|log
operator|->
name|list
operator|.
name|items
argument_list|,
name|log
operator|->
name|list
operator|.
name|nr
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|string_list_item
argument_list|)
argument_list|,
name|log
operator|->
name|summary
condition|?
name|compare_by_counter
else|:
name|compare_by_list
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|log
operator|->
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|string_list_item
modifier|*
name|item
init|=
operator|&
name|log
operator|->
name|list
operator|.
name|items
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|log
operator|->
name|summary
condition|)
block|{
name|printf
argument_list|(
literal|"%6d\t%s\n"
argument_list|,
operator|(
name|int
operator|)
name|UTIL_TO_INT
argument_list|(
name|item
argument_list|)
argument_list|,
name|item
operator|->
name|string
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|struct
name|string_list
modifier|*
name|onelines
init|=
name|item
operator|->
name|util
decl_stmt|;
name|printf
argument_list|(
literal|"%s (%d):\n"
argument_list|,
name|item
operator|->
name|string
argument_list|,
name|onelines
operator|->
name|nr
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
name|onelines
operator|->
name|nr
operator|-
literal|1
init|;
name|j
operator|>=
literal|0
condition|;
name|j
operator|--
control|)
block|{
specifier|const
name|char
modifier|*
name|msg
init|=
name|onelines
operator|->
name|items
index|[
name|j
index|]
operator|.
name|string
decl_stmt|;
if|if
condition|(
name|log
operator|->
name|wrap_lines
condition|)
block|{
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|add_wrapped_shortlog_msg
argument_list|(
operator|&
name|sb
argument_list|,
name|msg
argument_list|,
name|log
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|sb
operator|.
name|len
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"      %s\n"
argument_list|,
name|msg
argument_list|)
expr_stmt|;
block|}
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|onelines
operator|->
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear
argument_list|(
name|onelines
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|onelines
argument_list|)
expr_stmt|;
block|}
name|log
operator|->
name|list
operator|.
name|items
index|[
name|i
index|]
operator|.
name|util
operator|=
name|NULL
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|log
operator|->
name|list
operator|.
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|log
operator|->
name|list
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|clear_mailmap
argument_list|(
operator|&
name|log
operator|->
name|mailmap
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
