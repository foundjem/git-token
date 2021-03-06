begin_unit
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
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
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_include
include|#
directive|include
file|"sha1-array.h"
end_include
begin_comment
comment|/*----- some often used options -----*/
end_comment
begin_function
DECL|function|parse_opt_abbrev_cb
name|int
name|parse_opt_abbrev_cb
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
name|int
name|v
decl_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
block|{
name|v
operator|=
name|unset
condition|?
literal|0
else|:
name|DEFAULT_ABBREV
expr_stmt|;
block|}
else|else
block|{
name|v
operator|=
name|strtol
argument_list|(
name|arg
argument_list|,
operator|(
name|char
operator|*
operator|*
operator|)
operator|&
name|arg
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|arg
condition|)
return|return
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"expects a numerical value"
argument_list|,
literal|0
argument_list|)
return|;
if|if
condition|(
name|v
operator|&&
name|v
operator|<
name|MINIMUM_ABBREV
condition|)
name|v
operator|=
name|MINIMUM_ABBREV
expr_stmt|;
elseif|else
if|if
condition|(
name|v
operator|>
literal|40
condition|)
name|v
operator|=
literal|40
expr_stmt|;
block|}
operator|*
operator|(
name|int
operator|*
operator|)
operator|(
name|opt
operator|->
name|value
operator|)
operator|=
name|v
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_approxidate_cb
name|int
name|parse_opt_approxidate_cb
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
operator|*
operator|(
name|unsigned
name|long
operator|*
operator|)
operator|(
name|opt
operator|->
name|value
operator|)
operator|=
name|approxidate
argument_list|(
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_expiry_date_cb
name|int
name|parse_opt_expiry_date_cb
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
return|return
name|parse_expiry_date
argument_list|(
name|arg
argument_list|,
operator|(
name|unsigned
name|long
operator|*
operator|)
name|opt
operator|->
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_color_flag_cb
name|int
name|parse_opt_color_flag_cb
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
name|int
name|value
decl_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
name|arg
operator|=
name|unset
condition|?
literal|"never"
else|:
operator|(
specifier|const
name|char
operator|*
operator|)
name|opt
operator|->
name|defval
expr_stmt|;
name|value
operator|=
name|git_config_colorbool
argument_list|(
name|NULL
argument_list|,
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
name|value
operator|<
literal|0
condition|)
return|return
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"expects \"always\", \"auto\", or \"never\""
argument_list|,
literal|0
argument_list|)
return|;
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
name|value
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_verbosity_cb
name|int
name|parse_opt_verbosity_cb
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
name|int
modifier|*
name|target
init|=
name|opt
operator|->
name|value
decl_stmt|;
if|if
condition|(
name|unset
condition|)
comment|/* --no-quiet, --no-verbose */
operator|*
name|target
operator|=
literal|0
expr_stmt|;
elseif|else
if|if
condition|(
name|opt
operator|->
name|short_name
operator|==
literal|'v'
condition|)
block|{
if|if
condition|(
operator|*
name|target
operator|>=
literal|0
condition|)
operator|(
operator|*
name|target
operator|)
operator|++
expr_stmt|;
else|else
operator|*
name|target
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
operator|*
name|target
operator|<=
literal|0
condition|)
operator|(
operator|*
name|target
operator|)
operator|--
expr_stmt|;
else|else
operator|*
name|target
operator|=
operator|-
literal|1
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_commits
name|int
name|parse_opt_commits
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
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"malformed object name %s"
argument_list|,
name|arg
argument_list|)
return|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
return|return
name|error
argument_list|(
literal|"no such commit %s"
argument_list|,
name|arg
argument_list|)
return|;
name|commit_list_insert
argument_list|(
name|commit
argument_list|,
name|opt
operator|->
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_object_name
name|int
name|parse_opt_object_name
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
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|unset
condition|)
block|{
name|sha1_array_clear
argument_list|(
name|opt
operator|->
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|arg
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"malformed object name '%s'"
argument_list|)
argument_list|,
name|arg
argument_list|)
return|;
name|sha1_array_append
argument_list|(
name|opt
operator|->
name|value
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_tertiary
name|int
name|parse_opt_tertiary
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
name|int
modifier|*
name|target
init|=
name|opt
operator|->
name|value
decl_stmt|;
operator|*
name|target
operator|=
name|unset
condition|?
literal|2
else|:
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_options_concat
name|int
name|parse_options_concat
parameter_list|(
name|struct
name|option
modifier|*
name|dst
parameter_list|,
name|size_t
name|dst_size
parameter_list|,
name|struct
name|option
modifier|*
name|src
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|dst_size
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|dst
index|[
name|i
index|]
operator|.
name|type
operator|==
name|OPTION_END
condition|)
break|break;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|i
operator|<
name|dst_size
condition|;
name|i
operator|++
operator|,
name|j
operator|++
control|)
block|{
name|dst
index|[
name|i
index|]
operator|=
name|src
index|[
name|j
index|]
expr_stmt|;
if|if
condition|(
name|src
index|[
name|j
index|]
operator|.
name|type
operator|==
name|OPTION_END
condition|)
return|return
literal|0
return|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_string_list
name|int
name|parse_opt_string_list
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
name|string_list
modifier|*
name|v
init|=
name|opt
operator|->
name|value
decl_stmt|;
if|if
condition|(
name|unset
condition|)
block|{
name|string_list_clear
argument_list|(
name|v
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|arg
condition|)
return|return
operator|-
literal|1
return|;
name|string_list_append
argument_list|(
name|v
argument_list|,
name|arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_opt_noop_cb
name|int
name|parse_opt_noop_cb
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
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/**  * Recreates the command-line option in the strbuf.  */
end_comment
begin_function
DECL|function|recreate_opt
specifier|static
name|int
name|recreate_opt
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
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
name|strbuf_reset
argument_list|(
name|sb
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|long_name
condition|)
block|{
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|unset
condition|?
literal|"--no-"
else|:
literal|"--"
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|opt
operator|->
name|long_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|arg
condition|)
block|{
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'='
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|arg
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|opt
operator|->
name|short_name
operator|&&
operator|!
name|unset
condition|)
block|{
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'-'
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
name|opt
operator|->
name|short_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|arg
condition|)
name|strbuf_addstr
argument_list|(
name|sb
argument_list|,
name|arg
argument_list|)
expr_stmt|;
block|}
else|else
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/**  * For an option opt, recreates the command-line option in opt->value which  * must be an char* initialized to NULL. This is useful when we need to pass  * the command-line option to another command. Since any previous value will be  * overwritten, this callback should only be used for options where the last  * one wins.  */
end_comment
begin_function
DECL|function|parse_opt_passthru
name|int
name|parse_opt_passthru
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
specifier|static
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|char
modifier|*
modifier|*
name|opt_value
init|=
name|opt
operator|->
name|value
decl_stmt|;
if|if
condition|(
name|recreate_opt
argument_list|(
operator|&
name|sb
argument_list|,
name|opt
argument_list|,
name|arg
argument_list|,
name|unset
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|*
name|opt_value
condition|)
name|free
argument_list|(
operator|*
name|opt_value
argument_list|)
expr_stmt|;
operator|*
name|opt_value
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|sb
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/**  * For an option opt, recreate the command-line option, appending it to  * opt->value which must be a argv_array. This is useful when we need to pass  * the command-line option, which can be specified multiple times, to another  * command.  */
end_comment
begin_function
DECL|function|parse_opt_passthru_argv
name|int
name|parse_opt_passthru_argv
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
specifier|static
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|argv_array
modifier|*
name|opt_value
init|=
name|opt
operator|->
name|value
decl_stmt|;
if|if
condition|(
name|recreate_opt
argument_list|(
operator|&
name|sb
argument_list|,
name|opt
argument_list|,
name|arg
argument_list|,
name|unset
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|argv_array_push
argument_list|(
name|opt_value
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
