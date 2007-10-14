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
begin_define
DECL|macro|OPT_SHORT
define|#
directive|define
name|OPT_SHORT
value|1
end_define
begin_define
DECL|macro|OPT_UNSET
define|#
directive|define
name|OPT_UNSET
value|2
end_define
begin_struct
DECL|struct|optparse_t
struct|struct
name|optparse_t
block|{
DECL|member|argv
specifier|const
name|char
modifier|*
modifier|*
name|argv
decl_stmt|;
DECL|member|argc
name|int
name|argc
decl_stmt|;
DECL|member|opt
specifier|const
name|char
modifier|*
name|opt
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|get_arg
specifier|static
specifier|inline
specifier|const
name|char
modifier|*
name|get_arg
parameter_list|(
name|struct
name|optparse_t
modifier|*
name|p
parameter_list|)
block|{
if|if
condition|(
name|p
operator|->
name|opt
condition|)
block|{
specifier|const
name|char
modifier|*
name|res
init|=
name|p
operator|->
name|opt
decl_stmt|;
name|p
operator|->
name|opt
operator|=
name|NULL
expr_stmt|;
return|return
name|res
return|;
block|}
name|p
operator|->
name|argc
operator|--
expr_stmt|;
return|return
operator|*
operator|++
name|p
operator|->
name|argv
return|;
block|}
end_function
begin_function
DECL|function|skip_prefix
specifier|static
specifier|inline
specifier|const
name|char
modifier|*
name|skip_prefix
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|size_t
name|len
init|=
name|strlen
argument_list|(
name|prefix
argument_list|)
decl_stmt|;
return|return
name|strncmp
argument_list|(
name|str
argument_list|,
name|prefix
argument_list|,
name|len
argument_list|)
condition|?
name|NULL
else|:
name|str
operator|+
name|len
return|;
block|}
end_function
begin_function
DECL|function|opterror
specifier|static
name|int
name|opterror
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
name|reason
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
if|if
condition|(
name|flags
operator|&
name|OPT_SHORT
condition|)
return|return
name|error
argument_list|(
literal|"switch `%c' %s"
argument_list|,
name|opt
operator|->
name|short_name
argument_list|,
name|reason
argument_list|)
return|;
if|if
condition|(
name|flags
operator|&
name|OPT_UNSET
condition|)
return|return
name|error
argument_list|(
literal|"option `no-%s' %s"
argument_list|,
name|opt
operator|->
name|long_name
argument_list|,
name|reason
argument_list|)
return|;
return|return
name|error
argument_list|(
literal|"option `%s' %s"
argument_list|,
name|opt
operator|->
name|long_name
argument_list|,
name|reason
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|get_value
specifier|static
name|int
name|get_value
parameter_list|(
name|struct
name|optparse_t
modifier|*
name|p
parameter_list|,
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|s
decl_stmt|,
modifier|*
name|arg
decl_stmt|;
name|arg
operator|=
name|p
operator|->
name|opt
condition|?
name|p
operator|->
name|opt
else|:
operator|(
name|p
operator|->
name|argc
operator|>
literal|1
condition|?
name|p
operator|->
name|argv
index|[
literal|1
index|]
else|:
name|NULL
operator|)
expr_stmt|;
if|if
condition|(
name|p
operator|->
name|opt
operator|&&
operator|(
name|flags
operator|&
name|OPT_UNSET
operator|)
condition|)
return|return
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"takes no value"
argument_list|,
name|flags
argument_list|)
return|;
switch|switch
condition|(
name|opt
operator|->
name|type
condition|)
block|{
case|case
name|OPTION_BOOLEAN
case|:
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|OPT_SHORT
operator|)
operator|&&
name|p
operator|->
name|opt
condition|)
return|return
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"takes no value"
argument_list|,
name|flags
argument_list|)
return|;
if|if
condition|(
name|flags
operator|&
name|OPT_UNSET
condition|)
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
literal|0
expr_stmt|;
else|else
operator|(
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|)
operator|++
expr_stmt|;
return|return
literal|0
return|;
case|case
name|OPTION_STRING
case|:
if|if
condition|(
name|flags
operator|&
name|OPT_UNSET
condition|)
block|{
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
operator|(
specifier|const
name|char
operator|*
operator|)
name|NULL
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|opt
operator|->
name|flags
operator|&
name|PARSE_OPT_OPTARG
operator|&&
operator|(
operator|!
name|arg
operator|||
operator|*
name|arg
operator|==
literal|'-'
operator|)
condition|)
block|{
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
operator|(
specifier|const
name|char
operator|*
operator|)
name|opt
operator|->
name|defval
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
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"requires a value"
argument_list|,
name|flags
argument_list|)
return|;
operator|*
operator|(
specifier|const
name|char
operator|*
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
name|get_arg
argument_list|(
name|p
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
case|case
name|OPTION_CALLBACK
case|:
if|if
condition|(
name|flags
operator|&
name|OPT_UNSET
condition|)
return|return
call|(
modifier|*
name|opt
operator|->
name|callback
call|)
argument_list|(
name|opt
argument_list|,
name|NULL
argument_list|,
literal|1
argument_list|)
return|;
if|if
condition|(
name|opt
operator|->
name|flags
operator|&
name|PARSE_OPT_OPTARG
operator|&&
operator|(
operator|!
name|arg
operator|||
operator|*
name|arg
operator|==
literal|'-'
operator|)
condition|)
return|return
call|(
modifier|*
name|opt
operator|->
name|callback
call|)
argument_list|(
name|opt
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
return|;
if|if
condition|(
operator|!
name|arg
condition|)
return|return
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"requires a value"
argument_list|,
name|flags
argument_list|)
return|;
return|return
call|(
modifier|*
name|opt
operator|->
name|callback
call|)
argument_list|(
name|opt
argument_list|,
name|get_arg
argument_list|(
name|p
argument_list|)
argument_list|,
literal|0
argument_list|)
return|;
case|case
name|OPTION_INTEGER
case|:
if|if
condition|(
name|flags
operator|&
name|OPT_UNSET
condition|)
block|{
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|opt
operator|->
name|flags
operator|&
name|PARSE_OPT_OPTARG
operator|&&
operator|(
operator|!
name|arg
operator|||
operator|!
name|isdigit
argument_list|(
operator|*
name|arg
argument_list|)
operator|)
condition|)
block|{
operator|*
operator|(
name|int
operator|*
operator|)
name|opt
operator|->
name|value
operator|=
name|opt
operator|->
name|defval
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
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"requires a value"
argument_list|,
name|flags
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
name|strtol
argument_list|(
name|get_arg
argument_list|(
name|p
argument_list|)
argument_list|,
operator|(
name|char
operator|*
operator|*
operator|)
operator|&
name|s
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|s
condition|)
return|return
name|opterror
argument_list|(
name|opt
argument_list|,
literal|"expects a numerical value"
argument_list|,
name|flags
argument_list|)
return|;
return|return
literal|0
return|;
default|default:
name|die
argument_list|(
literal|"should not happen, someone must be hit on the forehead"
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|parse_short_opt
specifier|static
name|int
name|parse_short_opt
parameter_list|(
name|struct
name|optparse_t
modifier|*
name|p
parameter_list|,
specifier|const
name|struct
name|option
modifier|*
name|options
parameter_list|)
block|{
for|for
control|(
init|;
name|options
operator|->
name|type
operator|!=
name|OPTION_END
condition|;
name|options
operator|++
control|)
block|{
if|if
condition|(
name|options
operator|->
name|short_name
operator|==
operator|*
name|p
operator|->
name|opt
condition|)
block|{
name|p
operator|->
name|opt
operator|=
name|p
operator|->
name|opt
index|[
literal|1
index|]
condition|?
name|p
operator|->
name|opt
operator|+
literal|1
else|:
name|NULL
expr_stmt|;
return|return
name|get_value
argument_list|(
name|p
argument_list|,
name|options
argument_list|,
name|OPT_SHORT
argument_list|)
return|;
block|}
block|}
return|return
name|error
argument_list|(
literal|"unknown switch `%c'"
argument_list|,
operator|*
name|p
operator|->
name|opt
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_long_opt
specifier|static
name|int
name|parse_long_opt
parameter_list|(
name|struct
name|optparse_t
modifier|*
name|p
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
specifier|const
name|struct
name|option
modifier|*
name|options
parameter_list|)
block|{
for|for
control|(
init|;
name|options
operator|->
name|type
operator|!=
name|OPTION_END
condition|;
name|options
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|rest
decl_stmt|;
name|int
name|flags
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|options
operator|->
name|long_name
condition|)
continue|continue;
name|rest
operator|=
name|skip_prefix
argument_list|(
name|arg
argument_list|,
name|options
operator|->
name|long_name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rest
condition|)
block|{
if|if
condition|(
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"no-"
argument_list|,
literal|3
argument_list|)
condition|)
continue|continue;
name|flags
operator||=
name|OPT_UNSET
expr_stmt|;
name|rest
operator|=
name|skip_prefix
argument_list|(
name|arg
operator|+
literal|3
argument_list|,
name|options
operator|->
name|long_name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rest
condition|)
continue|continue;
block|}
if|if
condition|(
operator|*
name|rest
condition|)
block|{
if|if
condition|(
operator|*
name|rest
operator|!=
literal|'='
condition|)
continue|continue;
name|p
operator|->
name|opt
operator|=
name|rest
operator|+
literal|1
expr_stmt|;
block|}
return|return
name|get_value
argument_list|(
name|p
argument_list|,
name|options
argument_list|,
name|flags
argument_list|)
return|;
block|}
return|return
name|error
argument_list|(
literal|"unknown option `%s'"
argument_list|,
name|arg
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_options
name|int
name|parse_options
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
name|struct
name|option
modifier|*
name|options
parameter_list|,
specifier|const
name|char
modifier|*
specifier|const
name|usagestr
index|[]
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
name|struct
name|optparse_t
name|args
init|=
block|{
name|argv
operator|+
literal|1
block|,
name|argc
operator|-
literal|1
block|,
name|NULL
block|}
decl_stmt|;
name|int
name|j
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
name|args
operator|.
name|argc
condition|;
name|args
operator|.
name|argc
operator|--
operator|,
name|args
operator|.
name|argv
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|args
operator|.
name|argv
index|[
literal|0
index|]
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|!=
literal|'-'
operator|||
operator|!
name|arg
index|[
literal|1
index|]
condition|)
block|{
name|argv
index|[
name|j
operator|++
index|]
operator|=
name|args
operator|.
name|argv
index|[
literal|0
index|]
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|arg
index|[
literal|1
index|]
operator|!=
literal|'-'
condition|)
block|{
name|args
operator|.
name|opt
operator|=
name|arg
operator|+
literal|1
expr_stmt|;
do|do
block|{
if|if
condition|(
operator|*
name|args
operator|.
name|opt
operator|==
literal|'h'
condition|)
name|usage_with_options
argument_list|(
name|usagestr
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_short_opt
argument_list|(
operator|&
name|args
argument_list|,
name|options
argument_list|)
operator|<
literal|0
condition|)
name|usage_with_options
argument_list|(
name|usagestr
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
name|args
operator|.
name|opt
condition|)
do|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|arg
index|[
literal|2
index|]
condition|)
block|{
comment|/* "--" */
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|PARSE_OPT_KEEP_DASHDASH
operator|)
condition|)
block|{
name|args
operator|.
name|argc
operator|--
expr_stmt|;
name|args
operator|.
name|argv
operator|++
expr_stmt|;
block|}
break|break;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
operator|+
literal|2
argument_list|,
literal|"help"
argument_list|)
condition|)
name|usage_with_options
argument_list|(
name|usagestr
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_long_opt
argument_list|(
operator|&
name|args
argument_list|,
name|arg
operator|+
literal|2
argument_list|,
name|options
argument_list|)
condition|)
name|usage_with_options
argument_list|(
name|usagestr
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
name|memmove
argument_list|(
name|argv
operator|+
name|j
argument_list|,
name|args
operator|.
name|argv
argument_list|,
name|args
operator|.
name|argc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|argv
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
name|j
operator|+
name|args
operator|.
name|argc
index|]
operator|=
name|NULL
expr_stmt|;
return|return
name|j
operator|+
name|args
operator|.
name|argc
return|;
block|}
end_function
begin_define
DECL|macro|USAGE_OPTS_WIDTH
define|#
directive|define
name|USAGE_OPTS_WIDTH
value|24
end_define
begin_define
DECL|macro|USAGE_GAP
define|#
directive|define
name|USAGE_GAP
value|2
end_define
begin_function
DECL|function|usage_with_options
name|void
name|usage_with_options
parameter_list|(
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|usagestr
parameter_list|,
specifier|const
name|struct
name|option
modifier|*
name|opts
parameter_list|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"usage: %s\n"
argument_list|,
operator|*
name|usagestr
operator|++
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|usagestr
operator|&&
operator|*
operator|*
name|usagestr
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"   or: %s\n"
argument_list|,
operator|*
name|usagestr
operator|++
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|usagestr
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"    %s\n"
argument_list|,
operator|*
name|usagestr
operator|++
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|type
operator|!=
name|OPTION_GROUP
condition|)
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
for|for
control|(
init|;
name|opts
operator|->
name|type
operator|!=
name|OPTION_END
condition|;
name|opts
operator|++
control|)
block|{
name|size_t
name|pos
decl_stmt|;
name|int
name|pad
decl_stmt|;
if|if
condition|(
name|opts
operator|->
name|type
operator|==
name|OPTION_GROUP
condition|)
block|{
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|opts
operator|->
name|help
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s\n"
argument_list|,
name|opts
operator|->
name|help
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|pos
operator|=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"    "
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|short_name
condition|)
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"-%c"
argument_list|,
name|opts
operator|->
name|short_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|long_name
operator|&&
name|opts
operator|->
name|short_name
condition|)
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|", "
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|long_name
condition|)
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"--%s"
argument_list|,
name|opts
operator|->
name|long_name
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|opts
operator|->
name|type
condition|)
block|{
case|case
name|OPTION_INTEGER
case|:
if|if
condition|(
name|opts
operator|->
name|flags
operator|&
name|PARSE_OPT_OPTARG
condition|)
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" [<n>]"
argument_list|)
expr_stmt|;
else|else
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"<n>"
argument_list|)
expr_stmt|;
break|break;
case|case
name|OPTION_STRING
case|:
case|case
name|OPTION_CALLBACK
case|:
if|if
condition|(
name|opts
operator|->
name|argh
condition|)
block|{
if|if
condition|(
name|opts
operator|->
name|flags
operator|&
name|PARSE_OPT_OPTARG
condition|)
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" [<%s>]"
argument_list|,
name|opts
operator|->
name|argh
argument_list|)
expr_stmt|;
else|else
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"<%s>"
argument_list|,
name|opts
operator|->
name|argh
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|opts
operator|->
name|flags
operator|&
name|PARSE_OPT_OPTARG
condition|)
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" [...]"
argument_list|)
expr_stmt|;
else|else
name|pos
operator|+=
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" ..."
argument_list|)
expr_stmt|;
block|}
break|break;
default|default:
break|break;
block|}
if|if
condition|(
name|pos
operator|<=
name|USAGE_OPTS_WIDTH
condition|)
name|pad
operator|=
name|USAGE_OPTS_WIDTH
operator|-
name|pos
expr_stmt|;
else|else
block|{
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|pad
operator|=
name|USAGE_OPTS_WIDTH
expr_stmt|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%*s%s\n"
argument_list|,
name|pad
operator|+
name|USAGE_GAP
argument_list|,
literal|""
argument_list|,
name|opts
operator|->
name|help
argument_list|)
expr_stmt|;
block|}
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|129
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*----- some often used options -----*/
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
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
end_unit
