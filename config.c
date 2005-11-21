begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  * Copyright (C) Johannes Schindelin, 2005  *  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<regex.h>
end_include
begin_define
DECL|macro|MAXNAME
define|#
directive|define
name|MAXNAME
value|(256)
end_define
begin_decl_stmt
DECL|variable|config_file
specifier|static
name|FILE
modifier|*
name|config_file
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|config_linenr
specifier|static
name|int
name|config_linenr
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_next_char
specifier|static
name|int
name|get_next_char
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|c
decl_stmt|;
name|FILE
modifier|*
name|f
decl_stmt|;
name|c
operator|=
literal|'\n'
expr_stmt|;
if|if
condition|(
operator|(
name|f
operator|=
name|config_file
operator|)
operator|!=
name|NULL
condition|)
block|{
name|c
operator|=
name|fgetc
argument_list|(
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
name|c
operator|==
literal|'\r'
condition|)
block|{
comment|/* DOS like systems */
name|c
operator|=
name|fgetc
argument_list|(
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
name|c
operator|!=
literal|'\n'
condition|)
block|{
name|ungetc
argument_list|(
name|c
argument_list|,
name|f
argument_list|)
expr_stmt|;
name|c
operator|=
literal|'\r'
expr_stmt|;
block|}
block|}
if|if
condition|(
name|c
operator|==
literal|'\n'
condition|)
name|config_linenr
operator|++
expr_stmt|;
if|if
condition|(
name|c
operator|==
name|EOF
condition|)
block|{
name|config_file
operator|=
name|NULL
expr_stmt|;
name|c
operator|=
literal|'\n'
expr_stmt|;
block|}
block|}
return|return
name|c
return|;
block|}
end_function
begin_function
DECL|function|parse_value
specifier|static
name|char
modifier|*
name|parse_value
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
name|value
index|[
literal|1024
index|]
decl_stmt|;
name|int
name|quote
init|=
literal|0
decl_stmt|,
name|comment
init|=
literal|0
decl_stmt|,
name|len
init|=
literal|0
decl_stmt|,
name|space
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|c
init|=
name|get_next_char
argument_list|()
decl_stmt|;
if|if
condition|(
name|len
operator|>=
sizeof|sizeof
argument_list|(
name|value
argument_list|)
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|c
operator|==
literal|'\n'
condition|)
block|{
if|if
condition|(
name|quote
condition|)
return|return
name|NULL
return|;
name|value
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|value
return|;
block|}
if|if
condition|(
name|comment
condition|)
continue|continue;
if|if
condition|(
name|isspace
argument_list|(
name|c
argument_list|)
operator|&&
operator|!
name|quote
condition|)
block|{
name|space
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|space
condition|)
block|{
if|if
condition|(
name|len
condition|)
name|value
index|[
name|len
operator|++
index|]
operator|=
literal|' '
expr_stmt|;
name|space
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|c
operator|==
literal|'\\'
condition|)
block|{
name|c
operator|=
name|get_next_char
argument_list|()
expr_stmt|;
switch|switch
condition|(
name|c
condition|)
block|{
case|case
literal|'\n'
case|:
continue|continue;
case|case
literal|'t'
case|:
name|c
operator|=
literal|'\t'
expr_stmt|;
break|break;
case|case
literal|'b'
case|:
name|c
operator|=
literal|'\b'
expr_stmt|;
break|break;
case|case
literal|'n'
case|:
name|c
operator|=
literal|'\n'
expr_stmt|;
break|break;
comment|/* Some characters escape as themselves */
case|case
literal|'\\'
case|:
case|case
literal|'"'
case|:
break|break;
comment|/* Reject unknown escape sequences */
default|default:
return|return
name|NULL
return|;
block|}
name|value
index|[
name|len
operator|++
index|]
operator|=
name|c
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|c
operator|==
literal|'"'
condition|)
block|{
name|quote
operator|=
literal|1
operator|-
name|quote
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|quote
condition|)
block|{
if|if
condition|(
name|c
operator|==
literal|';'
operator|||
name|c
operator|==
literal|'#'
condition|)
block|{
name|comment
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
block|}
name|value
index|[
name|len
operator|++
index|]
operator|=
name|c
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|get_value
specifier|static
name|int
name|get_value
parameter_list|(
name|config_fn_t
name|fn
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|int
name|len
parameter_list|)
block|{
name|int
name|c
decl_stmt|;
name|char
modifier|*
name|value
decl_stmt|;
comment|/* Get the full name */
for|for
control|(
init|;
condition|;
control|)
block|{
name|c
operator|=
name|get_next_char
argument_list|()
expr_stmt|;
if|if
condition|(
name|c
operator|==
name|EOF
condition|)
break|break;
if|if
condition|(
operator|!
name|isalnum
argument_list|(
name|c
argument_list|)
condition|)
break|break;
name|name
index|[
name|len
operator|++
index|]
operator|=
name|tolower
argument_list|(
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>=
name|MAXNAME
condition|)
return|return
operator|-
literal|1
return|;
block|}
name|name
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|c
operator|==
literal|' '
operator|||
name|c
operator|==
literal|'\t'
condition|)
name|c
operator|=
name|get_next_char
argument_list|()
expr_stmt|;
name|value
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|c
operator|!=
literal|'\n'
condition|)
block|{
if|if
condition|(
name|c
operator|!=
literal|'='
condition|)
return|return
operator|-
literal|1
return|;
name|value
operator|=
name|parse_value
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|value
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
name|fn
argument_list|(
name|name
argument_list|,
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|get_base_var
specifier|static
name|int
name|get_base_var
parameter_list|(
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|baselen
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|c
init|=
name|get_next_char
argument_list|()
decl_stmt|;
if|if
condition|(
name|c
operator|==
name|EOF
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|c
operator|==
literal|']'
condition|)
return|return
name|baselen
return|;
if|if
condition|(
operator|!
name|isalnum
argument_list|(
name|c
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|baselen
operator|>
name|MAXNAME
operator|/
literal|2
condition|)
return|return
operator|-
literal|1
return|;
name|name
index|[
name|baselen
operator|++
index|]
operator|=
name|tolower
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|git_parse_file
specifier|static
name|int
name|git_parse_file
parameter_list|(
name|config_fn_t
name|fn
parameter_list|)
block|{
name|int
name|comment
init|=
literal|0
decl_stmt|;
name|int
name|baselen
init|=
literal|0
decl_stmt|;
specifier|static
name|char
name|var
index|[
name|MAXNAME
index|]
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|c
init|=
name|get_next_char
argument_list|()
decl_stmt|;
if|if
condition|(
name|c
operator|==
literal|'\n'
condition|)
block|{
comment|/* EOF? */
if|if
condition|(
operator|!
name|config_file
condition|)
return|return
literal|0
return|;
name|comment
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|comment
operator|||
name|isspace
argument_list|(
name|c
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|c
operator|==
literal|'#'
operator|||
name|c
operator|==
literal|';'
condition|)
block|{
name|comment
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|c
operator|==
literal|'['
condition|)
block|{
name|baselen
operator|=
name|get_base_var
argument_list|(
name|var
argument_list|)
expr_stmt|;
if|if
condition|(
name|baselen
operator|<=
literal|0
condition|)
break|break;
name|var
index|[
name|baselen
operator|++
index|]
operator|=
literal|'.'
expr_stmt|;
name|var
index|[
name|baselen
index|]
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|isalpha
argument_list|(
name|c
argument_list|)
condition|)
break|break;
name|var
index|[
name|baselen
index|]
operator|=
name|tolower
argument_list|(
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_value
argument_list|(
name|fn
argument_list|,
name|var
argument_list|,
name|baselen
operator|+
literal|1
argument_list|)
operator|<
literal|0
condition|)
break|break;
block|}
name|die
argument_list|(
literal|"bad config file line %d"
argument_list|,
name|config_linenr
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_config_int
name|int
name|git_config_int
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
name|value
operator|&&
operator|*
name|value
condition|)
block|{
name|char
modifier|*
name|end
decl_stmt|;
name|int
name|val
init|=
name|strtol
argument_list|(
name|value
argument_list|,
operator|&
name|end
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
operator|*
name|end
condition|)
return|return
name|val
return|;
block|}
name|die
argument_list|(
literal|"bad config value for '%s'"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_config_bool
name|int
name|git_config_bool
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
literal|1
return|;
if|if
condition|(
operator|!
operator|*
name|value
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|value
argument_list|,
literal|"true"
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|value
argument_list|,
literal|"false"
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
name|git_config_int
argument_list|(
name|name
argument_list|,
name|value
argument_list|)
operator|!=
literal|0
return|;
block|}
end_function
begin_function
DECL|function|git_default_config
name|int
name|git_default_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
comment|/* This needs a better name */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"core.filemode"
argument_list|)
condition|)
block|{
name|trust_executable_bit
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
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
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"core.symrefsonly"
argument_list|)
condition|)
block|{
name|only_use_symrefs
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
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
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"user.name"
argument_list|)
condition|)
block|{
name|strncpy
argument_list|(
name|git_default_name
argument_list|,
name|value
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_name
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"user.email"
argument_list|)
condition|)
block|{
name|strncpy
argument_list|(
name|git_default_email
argument_list|,
name|value
argument_list|,
sizeof|sizeof
argument_list|(
name|git_default_email
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"diff.renamelimit"
argument_list|)
condition|)
block|{
name|diff_rename_limit_default
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* Add other config variables here.. */
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|git_config
name|int
name|git_config
parameter_list|(
name|config_fn_t
name|fn
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
name|FILE
modifier|*
name|f
init|=
name|fopen
argument_list|(
name|git_path
argument_list|(
literal|"config"
argument_list|)
argument_list|,
literal|"r"
argument_list|)
decl_stmt|;
name|ret
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|f
condition|)
block|{
name|config_file
operator|=
name|f
expr_stmt|;
name|config_linenr
operator|=
literal|1
expr_stmt|;
name|ret
operator|=
name|git_parse_file
argument_list|(
name|fn
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|f
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/*  * Find all the stuff for git_config_set() below.  */
end_comment
begin_define
DECL|macro|MAX_MATCHES
define|#
directive|define
name|MAX_MATCHES
value|512
end_define
begin_struct
specifier|static
struct|struct
block|{
DECL|member|baselen
name|int
name|baselen
decl_stmt|;
DECL|member|key
name|char
modifier|*
name|key
decl_stmt|;
DECL|member|do_not_match
name|int
name|do_not_match
decl_stmt|;
DECL|member|value_regex
name|regex_t
modifier|*
name|value_regex
decl_stmt|;
DECL|member|multi_replace
name|int
name|multi_replace
decl_stmt|;
DECL|member|offset
name|off_t
name|offset
index|[
name|MAX_MATCHES
index|]
decl_stmt|;
DECL|enumerator|START
DECL|enumerator|SECTION_SEEN
DECL|enumerator|SECTION_END_SEEN
DECL|enumerator|KEY_SEEN
DECL|member|state
enum|enum
block|{
name|START
block|,
name|SECTION_SEEN
block|,
name|SECTION_END_SEEN
block|,
name|KEY_SEEN
block|}
name|state
enum|;
DECL|member|seen
name|int
name|seen
decl_stmt|;
block|}
DECL|variable|store
name|store
struct|;
end_struct
begin_function
DECL|function|matches
specifier|static
name|int
name|matches
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
return|return
operator|!
name|strcmp
argument_list|(
name|key
argument_list|,
name|store
operator|.
name|key
argument_list|)
operator|&&
operator|(
name|store
operator|.
name|value_regex
operator|==
name|NULL
operator|||
operator|(
name|store
operator|.
name|do_not_match
operator|^
operator|!
name|regexec
argument_list|(
name|store
operator|.
name|value_regex
argument_list|,
name|value
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
operator|)
operator|)
return|;
block|}
end_function
begin_function
DECL|function|store_aux
specifier|static
name|int
name|store_aux
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
switch|switch
condition|(
name|store
operator|.
name|state
condition|)
block|{
case|case
name|KEY_SEEN
case|:
if|if
condition|(
name|matches
argument_list|(
name|key
argument_list|,
name|value
argument_list|)
condition|)
block|{
if|if
condition|(
name|store
operator|.
name|seen
operator|==
literal|1
operator|&&
name|store
operator|.
name|multi_replace
operator|==
literal|0
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Warning: %s has multiple values\n"
argument_list|,
name|key
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|store
operator|.
name|seen
operator|>=
name|MAX_MATCHES
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Too many matches\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|store
operator|.
name|offset
index|[
name|store
operator|.
name|seen
index|]
operator|=
name|ftell
argument_list|(
name|config_file
argument_list|)
expr_stmt|;
name|store
operator|.
name|seen
operator|++
expr_stmt|;
block|}
break|break;
case|case
name|SECTION_SEEN
case|:
if|if
condition|(
name|strncmp
argument_list|(
name|key
argument_list|,
name|store
operator|.
name|key
argument_list|,
name|store
operator|.
name|baselen
operator|+
literal|1
argument_list|)
condition|)
block|{
name|store
operator|.
name|state
operator|=
name|SECTION_END_SEEN
expr_stmt|;
break|break;
block|}
else|else
comment|/* do not increment matches: this is no match */
name|store
operator|.
name|offset
index|[
name|store
operator|.
name|seen
index|]
operator|=
name|ftell
argument_list|(
name|config_file
argument_list|)
expr_stmt|;
comment|/* fallthru */
case|case
name|SECTION_END_SEEN
case|:
case|case
name|START
case|:
if|if
condition|(
name|matches
argument_list|(
name|key
argument_list|,
name|value
argument_list|)
condition|)
block|{
name|store
operator|.
name|offset
index|[
name|store
operator|.
name|seen
index|]
operator|=
name|ftell
argument_list|(
name|config_file
argument_list|)
expr_stmt|;
name|store
operator|.
name|state
operator|=
name|KEY_SEEN
expr_stmt|;
name|store
operator|.
name|seen
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|key
argument_list|,
name|store
operator|.
name|key
argument_list|,
name|store
operator|.
name|baselen
argument_list|)
condition|)
name|store
operator|.
name|state
operator|=
name|SECTION_SEEN
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|store_write_section
specifier|static
name|void
name|store_write_section
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|char
modifier|*
name|key
parameter_list|)
block|{
name|write
argument_list|(
name|fd
argument_list|,
literal|"["
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|write
argument_list|(
name|fd
argument_list|,
name|key
argument_list|,
name|store
operator|.
name|baselen
argument_list|)
expr_stmt|;
name|write
argument_list|(
name|fd
argument_list|,
literal|"]\n"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|store_write_pair
specifier|static
name|void
name|store_write_pair
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|write
argument_list|(
name|fd
argument_list|,
literal|"\t"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|write
argument_list|(
name|fd
argument_list|,
name|key
operator|+
name|store
operator|.
name|baselen
operator|+
literal|1
argument_list|,
name|strlen
argument_list|(
name|key
operator|+
name|store
operator|.
name|baselen
operator|+
literal|1
argument_list|)
argument_list|)
expr_stmt|;
name|write
argument_list|(
name|fd
argument_list|,
literal|" = "
argument_list|,
literal|3
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|value
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
switch|switch
condition|(
name|value
index|[
name|i
index|]
condition|)
block|{
case|case
literal|'\n'
case|:
name|write
argument_list|(
name|fd
argument_list|,
literal|"\\n"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'\t'
case|:
name|write
argument_list|(
name|fd
argument_list|,
literal|"\\t"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'"'
case|:
case|case
literal|'\\'
case|:
name|write
argument_list|(
name|fd
argument_list|,
literal|"\\"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
default|default:
name|write
argument_list|(
name|fd
argument_list|,
name|value
operator|+
name|i
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|write
argument_list|(
name|fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|find_beginning_of_line
specifier|static
name|int
name|find_beginning_of_line
parameter_list|(
specifier|const
name|char
modifier|*
name|contents
parameter_list|,
name|int
name|size
parameter_list|,
name|int
name|offset_
parameter_list|,
name|int
modifier|*
name|found_bracket
parameter_list|)
block|{
name|int
name|equal_offset
init|=
name|size
decl_stmt|,
name|bracket_offset
init|=
name|size
decl_stmt|;
name|int
name|offset
decl_stmt|;
for|for
control|(
name|offset
operator|=
name|offset_
operator|-
literal|2
init|;
name|offset
operator|>
literal|0
operator|&&
name|contents
index|[
name|offset
index|]
operator|!=
literal|'\n'
condition|;
name|offset
operator|--
control|)
switch|switch
condition|(
name|contents
index|[
name|offset
index|]
condition|)
block|{
case|case
literal|'='
case|:
name|equal_offset
operator|=
name|offset
expr_stmt|;
break|break;
case|case
literal|']'
case|:
name|bracket_offset
operator|=
name|offset
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|bracket_offset
operator|<
name|equal_offset
condition|)
block|{
operator|*
name|found_bracket
operator|=
literal|1
expr_stmt|;
name|offset
operator|=
name|bracket_offset
operator|+
literal|1
expr_stmt|;
block|}
else|else
name|offset
operator|++
expr_stmt|;
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|git_config_set
name|int
name|git_config_set
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
return|return
name|git_config_set_multivar
argument_list|(
name|key
argument_list|,
name|value
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * If value==NULL, unset in (remove from) config,  * if value_regex!=NULL, disregard key/value pairs where value does not match.  * if multi_replace==0, nothing, or only one matching key/value is replaced,  *     else all matching key/values (regardless how many) are removed,  *     before the new pair is written.  *  * Returns 0 on success.  *  * This function does this:  *  * - it locks the config file by creating ".git/config.lock"  *  * - it then parses the config using store_aux() as validator to find  *   the position on the key/value pair to replace. If it is to be unset,  *   it must be found exactly once.  *  * - the config file is mmap()ed and the part before the match (if any) is  *   written to the lock file, then the changed part and the rest.  *  * - the config file is removed and the lock file rename()d to it.  *  */
end_comment
begin_function
DECL|function|git_config_set_multivar
name|int
name|git_config_set_multivar
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
specifier|const
name|char
modifier|*
name|value_regex
parameter_list|,
name|int
name|multi_replace
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|char
modifier|*
name|config_file
init|=
name|strdup
argument_list|(
name|git_path
argument_list|(
literal|"config"
argument_list|)
argument_list|)
decl_stmt|;
name|char
modifier|*
name|lock_file
init|=
name|strdup
argument_list|(
name|git_path
argument_list|(
literal|"config.lock"
argument_list|)
argument_list|)
decl_stmt|;
name|store
operator|.
name|multi_replace
operator|=
name|multi_replace
expr_stmt|;
comment|/* 	 * Since "key" actually contains the section name and the real 	 * key name separated by a dot, we have to know where the dot is. 	 */
for|for
control|(
name|store
operator|.
name|baselen
operator|=
literal|0
init|;
name|key
index|[
name|store
operator|.
name|baselen
index|]
operator|!=
literal|'.'
operator|&&
name|key
index|[
name|store
operator|.
name|baselen
index|]
condition|;
name|store
operator|.
name|baselen
operator|++
control|)
empty_stmt|;
if|if
condition|(
operator|!
name|key
index|[
name|store
operator|.
name|baselen
index|]
operator|||
operator|!
name|key
index|[
name|store
operator|.
name|baselen
operator|+
literal|1
index|]
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"key does not contain a section: %s\n"
argument_list|,
name|key
argument_list|)
expr_stmt|;
return|return
literal|2
return|;
block|}
comment|/* 	 * Validate the key and while at it, lower case it for matching. 	 */
name|store
operator|.
name|key
operator|=
operator|(
name|char
operator|*
operator|)
name|malloc
argument_list|(
name|strlen
argument_list|(
name|key
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|key
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|i
operator|!=
name|store
operator|.
name|baselen
operator|&&
operator|(
operator|!
name|isalnum
argument_list|(
name|key
index|[
name|i
index|]
argument_list|)
operator|||
operator|(
name|i
operator|==
name|store
operator|.
name|baselen
operator|+
literal|1
operator|&&
operator|!
name|isalpha
argument_list|(
name|key
index|[
name|i
index|]
argument_list|)
operator|)
operator|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"invalid key: %s\n"
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|store
operator|.
name|key
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
else|else
name|store
operator|.
name|key
index|[
name|i
index|]
operator|=
name|tolower
argument_list|(
name|key
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|store
operator|.
name|key
index|[
name|i
index|]
operator|=
literal|0
expr_stmt|;
comment|/* 	 * The lock_file serves a purpose in addition to locking: the new 	 * contents of .git/config will be written into it. 	 */
name|fd
operator|=
name|open
argument_list|(
name|lock_file
argument_list|,
name|O_WRONLY
operator||
name|O_CREAT
operator||
name|O_EXCL
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"could not lock config file\n"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|store
operator|.
name|key
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* 	 * If .git/config does not exist yet, write a minimal version. 	 */
if|if
condition|(
name|stat
argument_list|(
name|config_file
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
specifier|static
specifier|const
name|char
name|contents
index|[]
init|=
literal|"#\n"
literal|"# This is the config file\n"
literal|"#\n"
literal|"\n"
decl_stmt|;
name|free
argument_list|(
name|store
operator|.
name|key
argument_list|)
expr_stmt|;
comment|/* if nothing to unset, error out */
if|if
condition|(
name|value
operator|==
name|NULL
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|lock_file
argument_list|)
expr_stmt|;
return|return
literal|5
return|;
block|}
name|store
operator|.
name|key
operator|=
operator|(
name|char
operator|*
operator|)
name|key
expr_stmt|;
name|write
argument_list|(
name|fd
argument_list|,
name|contents
argument_list|,
sizeof|sizeof
argument_list|(
name|contents
argument_list|)
operator|-
literal|1
argument_list|)
expr_stmt|;
name|store_write_section
argument_list|(
name|fd
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|store_write_pair
argument_list|(
name|fd
argument_list|,
name|key
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|int
name|in_fd
decl_stmt|;
name|char
modifier|*
name|contents
decl_stmt|;
name|int
name|i
decl_stmt|,
name|copy_begin
decl_stmt|,
name|copy_end
decl_stmt|,
name|new_line
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|value_regex
operator|==
name|NULL
condition|)
name|store
operator|.
name|value_regex
operator|=
name|NULL
expr_stmt|;
else|else
block|{
if|if
condition|(
name|value_regex
index|[
literal|0
index|]
operator|==
literal|'!'
condition|)
block|{
name|store
operator|.
name|do_not_match
operator|=
literal|1
expr_stmt|;
name|value_regex
operator|++
expr_stmt|;
block|}
else|else
name|store
operator|.
name|do_not_match
operator|=
literal|0
expr_stmt|;
name|store
operator|.
name|value_regex
operator|=
operator|(
name|regex_t
operator|*
operator|)
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
name|regex_t
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|regcomp
argument_list|(
name|store
operator|.
name|value_regex
argument_list|,
name|value_regex
argument_list|,
name|REG_EXTENDED
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Invalid pattern: %s"
argument_list|,
name|value_regex
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|store
operator|.
name|value_regex
argument_list|)
expr_stmt|;
return|return
literal|6
return|;
block|}
block|}
name|store
operator|.
name|offset
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
name|store
operator|.
name|state
operator|=
name|START
expr_stmt|;
name|store
operator|.
name|seen
operator|=
literal|0
expr_stmt|;
comment|/* 		 * After this, store.offset will contain the *end* offset 		 * of the last match, or remain at 0 if no match was found. 		 * As a side effect, we make sure to transform only a valid 		 * existing config file. 		 */
if|if
condition|(
name|git_config
argument_list|(
name|store_aux
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"invalid config file\n"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|store
operator|.
name|key
argument_list|)
expr_stmt|;
if|if
condition|(
name|store
operator|.
name|value_regex
operator|!=
name|NULL
condition|)
block|{
name|regfree
argument_list|(
name|store
operator|.
name|value_regex
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|store
operator|.
name|value_regex
argument_list|)
expr_stmt|;
block|}
return|return
literal|3
return|;
block|}
name|free
argument_list|(
name|store
operator|.
name|key
argument_list|)
expr_stmt|;
if|if
condition|(
name|store
operator|.
name|value_regex
operator|!=
name|NULL
condition|)
block|{
name|regfree
argument_list|(
name|store
operator|.
name|value_regex
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|store
operator|.
name|value_regex
argument_list|)
expr_stmt|;
block|}
comment|/* if nothing to unset, or too many matches, error out */
if|if
condition|(
operator|(
name|store
operator|.
name|seen
operator|==
literal|0
operator|&&
name|value
operator|==
name|NULL
operator|)
operator|||
operator|(
name|store
operator|.
name|seen
operator|>
literal|1
operator|&&
name|multi_replace
operator|==
literal|0
operator|)
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|lock_file
argument_list|)
expr_stmt|;
return|return
literal|5
return|;
block|}
name|in_fd
operator|=
name|open
argument_list|(
name|config_file
argument_list|,
name|O_RDONLY
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
name|contents
operator|=
name|mmap
argument_list|(
name|NULL
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
name|PROT_READ
argument_list|,
name|MAP_PRIVATE
argument_list|,
name|in_fd
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|in_fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|store
operator|.
name|seen
operator|==
literal|0
condition|)
name|store
operator|.
name|seen
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
operator|,
name|copy_begin
operator|=
literal|0
init|;
name|i
operator|<
name|store
operator|.
name|seen
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|store
operator|.
name|offset
index|[
name|i
index|]
operator|==
literal|0
condition|)
block|{
name|store
operator|.
name|offset
index|[
name|i
index|]
operator|=
name|copy_end
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|store
operator|.
name|state
operator|!=
name|KEY_SEEN
condition|)
block|{
name|copy_end
operator|=
name|store
operator|.
name|offset
index|[
name|i
index|]
expr_stmt|;
block|}
else|else
name|copy_end
operator|=
name|find_beginning_of_line
argument_list|(
name|contents
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
name|store
operator|.
name|offset
index|[
name|i
index|]
operator|-
literal|2
argument_list|,
operator|&
name|new_line
argument_list|)
expr_stmt|;
comment|/* write the first part of the config */
if|if
condition|(
name|copy_end
operator|>
name|copy_begin
condition|)
block|{
name|write
argument_list|(
name|fd
argument_list|,
name|contents
operator|+
name|copy_begin
argument_list|,
name|copy_end
operator|-
name|copy_begin
argument_list|)
expr_stmt|;
if|if
condition|(
name|new_line
condition|)
name|write
argument_list|(
name|fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|copy_begin
operator|=
name|store
operator|.
name|offset
index|[
name|i
index|]
expr_stmt|;
block|}
comment|/* write the pair (value == NULL means unset) */
if|if
condition|(
name|value
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|store
operator|.
name|state
operator|==
name|START
condition|)
name|store_write_section
argument_list|(
name|fd
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|store_write_pair
argument_list|(
name|fd
argument_list|,
name|key
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
comment|/* write the rest of the config */
if|if
condition|(
name|copy_begin
operator|<
name|st
operator|.
name|st_size
condition|)
name|write
argument_list|(
name|fd
argument_list|,
name|contents
operator|+
name|copy_begin
argument_list|,
name|st
operator|.
name|st_size
operator|-
name|copy_begin
argument_list|)
expr_stmt|;
name|munmap
argument_list|(
name|contents
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|config_file
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|rename
argument_list|(
name|lock_file
argument_list|,
name|config_file
argument_list|)
operator|<
literal|0
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Could not rename the lock file?\n"
argument_list|)
expr_stmt|;
return|return
literal|4
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
