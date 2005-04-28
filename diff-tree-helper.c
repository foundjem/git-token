begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_function
DECL|function|matches_pathspec
specifier|static
name|int
name|matches_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|spec
parameter_list|,
name|int
name|cnt
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|cnt
condition|;
name|i
operator|++
control|)
block|{
name|int
name|speclen
init|=
name|strlen
argument_list|(
name|spec
index|[
name|i
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|spec
index|[
name|i
index|]
argument_list|,
name|name
argument_list|,
name|speclen
argument_list|)
operator|&&
name|speclen
operator|<=
name|namelen
operator|&&
operator|(
name|name
index|[
name|speclen
index|]
operator|==
literal|0
operator|||
name|name
index|[
name|speclen
index|]
operator|==
literal|'/'
operator|)
condition|)
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_oneside_change
specifier|static
name|int
name|parse_oneside_change
parameter_list|(
specifier|const
name|char
modifier|*
name|cp
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|one
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|ch
decl_stmt|;
name|one
operator|->
name|file_valid
operator|=
name|one
operator|->
name|sha1_valid
operator|=
literal|1
expr_stmt|;
name|one
operator|->
name|mode
operator|=
literal|0
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|cp
operator|)
operator|&&
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'7'
condition|)
block|{
name|one
operator|->
name|mode
operator|=
operator|(
name|one
operator|->
name|mode
operator|<<
literal|3
operator|)
operator||
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|strncmp
argument_list|(
name|cp
argument_list|,
literal|"\tblob\t"
argument_list|,
literal|6
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|6
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|cp
argument_list|,
name|one
operator|->
name|u
operator|.
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|40
expr_stmt|;
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
literal|'\t'
condition|)
return|return
operator|-
literal|1
return|;
name|strcpy
argument_list|(
name|path
argument_list|,
name|cp
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_diff_tree_output
specifier|static
name|int
name|parse_diff_tree_output
parameter_list|(
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|spec
parameter_list|,
name|int
name|cnt
parameter_list|)
block|{
name|struct
name|diff_spec
name|old
decl_stmt|,
name|new
decl_stmt|;
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
init|=
name|buf
decl_stmt|;
name|int
name|ch
decl_stmt|;
switch|switch
condition|(
operator|*
name|cp
operator|++
condition|)
block|{
case|case
literal|'U'
case|:
if|if
condition|(
operator|!
name|cnt
operator|||
name|matches_pathspec
argument_list|(
name|cp
operator|+
literal|1
argument_list|,
name|spec
argument_list|,
name|cnt
argument_list|)
condition|)
name|diff_unmerge
argument_list|(
name|cp
operator|+
literal|1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
case|case
literal|'+'
case|:
name|old
operator|.
name|file_valid
operator|=
literal|0
expr_stmt|;
name|parse_oneside_change
argument_list|(
name|cp
argument_list|,
operator|&
name|new
argument_list|,
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'-'
case|:
name|new
operator|.
name|file_valid
operator|=
literal|0
expr_stmt|;
name|parse_oneside_change
argument_list|(
name|cp
argument_list|,
operator|&
name|old
argument_list|,
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
literal|'*'
case|:
name|old
operator|.
name|file_valid
operator|=
name|old
operator|.
name|sha1_valid
operator|=
name|new
operator|.
name|file_valid
operator|=
name|new
operator|.
name|sha1_valid
operator|=
literal|1
expr_stmt|;
name|old
operator|.
name|mode
operator|=
name|new
operator|.
name|mode
operator|=
literal|0
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|cp
operator|)
operator|&&
operator|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'7'
operator|)
condition|)
block|{
name|old
operator|.
name|mode
operator|=
operator|(
name|old
operator|.
name|mode
operator|<<
literal|3
operator|)
operator||
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|strncmp
argument_list|(
name|cp
argument_list|,
literal|"->"
argument_list|,
literal|2
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|2
expr_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|cp
operator|)
operator|&&
operator|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'7'
operator|)
condition|)
block|{
name|new
operator|.
name|mode
operator|=
operator|(
name|new
operator|.
name|mode
operator|<<
literal|3
operator|)
operator||
operator|(
name|ch
operator|-
literal|'0'
operator|)
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|strncmp
argument_list|(
name|cp
argument_list|,
literal|"\tblob\t"
argument_list|,
literal|6
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|6
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|cp
argument_list|,
name|old
operator|.
name|u
operator|.
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|40
expr_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
name|cp
argument_list|,
literal|"->"
argument_list|,
literal|2
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|2
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|cp
argument_list|,
name|new
operator|.
name|u
operator|.
name|sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|cp
operator|+=
literal|40
expr_stmt|;
if|if
condition|(
operator|*
name|cp
operator|++
operator|!=
literal|'\t'
condition|)
return|return
operator|-
literal|1
return|;
name|strcpy
argument_list|(
name|path
argument_list|,
name|cp
argument_list|)
expr_stmt|;
break|break;
default|default:
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|cnt
operator|||
name|matches_pathspec
argument_list|(
name|path
argument_list|,
name|spec
argument_list|,
name|cnt
argument_list|)
condition|)
name|run_external_diff
argument_list|(
name|path
argument_list|,
operator|&
name|old
argument_list|,
operator|&
name|new
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|diff_tree_helper_usage
specifier|static
specifier|const
name|char
modifier|*
name|diff_tree_helper_usage
init|=
literal|"diff-tree-helper [-R] [-z] paths..."
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
decl_stmt|;
name|int
name|reverse_diff
init|=
literal|0
decl_stmt|;
name|int
name|line_termination
init|=
literal|'\n'
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
operator|<
name|ac
operator|&&
name|av
index|[
literal|1
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'R'
condition|)
name|reverse_diff
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|av
index|[
literal|1
index|]
index|[
literal|1
index|]
operator|==
literal|'z'
condition|)
name|line_termination
operator|=
literal|0
expr_stmt|;
else|else
name|usage
argument_list|(
name|diff_tree_helper_usage
argument_list|)
expr_stmt|;
name|ac
operator|--
expr_stmt|;
name|av
operator|++
expr_stmt|;
block|}
comment|/* the remaining parameters are paths patterns */
while|while
condition|(
literal|1
condition|)
block|{
name|int
name|status
decl_stmt|;
name|read_line
argument_list|(
operator|&
name|sb
argument_list|,
name|stdin
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
if|if
condition|(
name|sb
operator|.
name|eof
condition|)
break|break;
name|status
operator|=
name|parse_diff_tree_output
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|av
operator|+
literal|1
argument_list|,
name|ac
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|status
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cannot parse %s\n"
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
