begin_unit
begin_include
include|#
directive|include
file|<ctype.h>
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|commit_type
specifier|const
name|char
modifier|*
name|commit_type
init|=
literal|"commit"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_commit_format
name|enum
name|cmit_fmt
name|get_commit_format
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
operator|!
operator|*
name|arg
condition|)
return|return
name|CMIT_FMT_DEFAULT
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=raw"
argument_list|)
condition|)
return|return
name|CMIT_FMT_RAW
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=medium"
argument_list|)
condition|)
return|return
name|CMIT_FMT_MEDIUM
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=short"
argument_list|)
condition|)
return|return
name|CMIT_FMT_SHORT
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"=full"
argument_list|)
condition|)
return|return
name|CMIT_FMT_FULL
return|;
name|die
argument_list|(
literal|"invalid --pretty format"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_commit
specifier|static
name|struct
name|commit
modifier|*
name|check_commit
parameter_list|(
name|struct
name|object
modifier|*
name|obj
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|obj
operator|->
name|type
operator|!=
name|commit_type
condition|)
block|{
name|error
argument_list|(
literal|"Object %s is a %s, not a commit"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|obj
operator|->
name|type
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
return|;
block|}
end_function
begin_function
DECL|function|lookup_commit_reference
name|struct
name|commit
modifier|*
name|lookup_commit_reference
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
return|return
name|NULL
return|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|tag_type
condition|)
name|obj
operator|=
operator|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
operator|)
operator|->
name|tagged
expr_stmt|;
return|return
name|check_commit
argument_list|(
name|obj
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|lookup_commit
name|struct
name|commit
modifier|*
name|lookup_commit
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
block|{
name|struct
name|commit
modifier|*
name|ret
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit
argument_list|)
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|ret
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|commit
argument_list|)
argument_list|)
expr_stmt|;
name|created_object
argument_list|(
name|sha1
argument_list|,
operator|&
name|ret
operator|->
name|object
argument_list|)
expr_stmt|;
name|ret
operator|->
name|object
operator|.
name|type
operator|=
name|commit_type
expr_stmt|;
return|return
name|ret
return|;
block|}
if|if
condition|(
operator|!
name|obj
operator|->
name|type
condition|)
name|obj
operator|->
name|type
operator|=
name|commit_type
expr_stmt|;
return|return
name|check_commit
argument_list|(
name|obj
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_commit_date
specifier|static
name|unsigned
name|long
name|parse_commit_date
parameter_list|(
specifier|const
name|char
modifier|*
name|buf
parameter_list|)
block|{
name|unsigned
name|long
name|date
decl_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buf
argument_list|,
literal|"author"
argument_list|,
literal|6
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|buf
operator|++
operator|!=
literal|'\n'
condition|)
comment|/* nada */
empty_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|buf
argument_list|,
literal|"committer"
argument_list|,
literal|9
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|buf
operator|++
operator|!=
literal|'>'
condition|)
comment|/* nada */
empty_stmt|;
name|date
operator|=
name|strtoul
argument_list|(
name|buf
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|date
operator|==
name|ULONG_MAX
condition|)
name|date
operator|=
literal|0
expr_stmt|;
return|return
name|date
return|;
block|}
end_function
begin_function
DECL|function|parse_commit_buffer
name|int
name|parse_commit_buffer
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|void
modifier|*
name|bufptr
init|=
name|buffer
decl_stmt|;
name|unsigned
name|char
name|parent
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit_list
modifier|*
modifier|*
name|pptr
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|item
operator|->
name|object
operator|.
name|parsed
operator|=
literal|1
expr_stmt|;
name|get_sha1_hex
argument_list|(
name|bufptr
operator|+
literal|5
argument_list|,
name|parent
argument_list|)
expr_stmt|;
name|item
operator|->
name|tree
operator|=
name|lookup_tree
argument_list|(
name|parent
argument_list|)
expr_stmt|;
if|if
condition|(
name|item
operator|->
name|tree
condition|)
name|add_ref
argument_list|(
operator|&
name|item
operator|->
name|object
argument_list|,
operator|&
name|item
operator|->
name|tree
operator|->
name|object
argument_list|)
expr_stmt|;
name|bufptr
operator|+=
literal|46
expr_stmt|;
comment|/* "tree " + "hex sha1" + "\n" */
name|pptr
operator|=
operator|&
name|item
operator|->
name|parents
expr_stmt|;
while|while
condition|(
operator|!
name|memcmp
argument_list|(
name|bufptr
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|bufptr
operator|+
literal|7
argument_list|,
name|parent
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|new_parent
init|=
name|lookup_commit
argument_list|(
name|parent
argument_list|)
decl_stmt|;
if|if
condition|(
name|new_parent
condition|)
block|{
name|pptr
operator|=
operator|&
name|commit_list_insert
argument_list|(
name|new_parent
argument_list|,
name|pptr
argument_list|)
operator|->
name|next
expr_stmt|;
name|add_ref
argument_list|(
operator|&
name|item
operator|->
name|object
argument_list|,
operator|&
name|new_parent
operator|->
name|object
argument_list|)
expr_stmt|;
block|}
name|bufptr
operator|+=
literal|48
expr_stmt|;
block|}
name|item
operator|->
name|date
operator|=
name|parse_commit_date
argument_list|(
name|bufptr
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_commit
name|int
name|parse_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|)
block|{
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|ret
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
name|error
argument_list|(
literal|"Could not read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|strcmp
argument_list|(
name|type
argument_list|,
name|commit_type
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"Object %s not a commit"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
block|}
name|ret
operator|=
name|parse_commit_buffer
argument_list|(
name|item
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
name|item
operator|->
name|buffer
operator|=
name|buffer
expr_stmt|;
return|return
literal|0
return|;
block|}
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|commit_list_insert
name|struct
name|commit_list
modifier|*
name|commit_list_insert
parameter_list|(
name|struct
name|commit
modifier|*
name|item
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|list_p
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|new_list
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit_list
argument_list|)
argument_list|)
decl_stmt|;
name|new_list
operator|->
name|item
operator|=
name|item
expr_stmt|;
name|new_list
operator|->
name|next
operator|=
operator|*
name|list_p
expr_stmt|;
operator|*
name|list_p
operator|=
name|new_list
expr_stmt|;
return|return
name|new_list
return|;
block|}
end_function
begin_function
DECL|function|free_commit_list
name|void
name|free_commit_list
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|temp
init|=
name|list
decl_stmt|;
name|list
operator|=
name|temp
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|temp
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|insert_by_date
name|void
name|insert_by_date
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|struct
name|commit
modifier|*
name|item
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
modifier|*
name|pp
init|=
name|list
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
while|while
condition|(
operator|(
name|p
operator|=
operator|*
name|pp
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|p
operator|->
name|item
operator|->
name|date
operator|<
name|item
operator|->
name|date
condition|)
block|{
break|break;
block|}
name|pp
operator|=
operator|&
name|p
operator|->
name|next
expr_stmt|;
block|}
name|commit_list_insert
argument_list|(
name|item
argument_list|,
name|pp
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sort_by_date
name|void
name|sort_by_date
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|ret
init|=
name|NULL
decl_stmt|;
while|while
condition|(
operator|*
name|list
condition|)
block|{
name|insert_by_date
argument_list|(
operator|&
name|ret
argument_list|,
operator|(
operator|*
name|list
operator|)
operator|->
name|item
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
operator|(
operator|*
name|list
operator|)
operator|->
name|next
expr_stmt|;
block|}
operator|*
name|list
operator|=
name|ret
expr_stmt|;
block|}
end_function
begin_function
DECL|function|pop_most_recent_commit
name|struct
name|commit
modifier|*
name|pop_most_recent_commit
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|unsigned
name|int
name|mark
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|ret
init|=
operator|(
operator|*
name|list
operator|)
operator|->
name|item
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|ret
operator|->
name|parents
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|old
init|=
operator|*
name|list
decl_stmt|;
operator|*
name|list
operator|=
operator|(
operator|*
name|list
operator|)
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|old
argument_list|)
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|parse_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|mark
operator|)
condition|)
block|{
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|mark
expr_stmt|;
name|insert_by_date
argument_list|(
name|list
argument_list|,
name|commit
argument_list|)
expr_stmt|;
block|}
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/*  * Generic support for pretty-printing the header  */
end_comment
begin_function
DECL|function|get_one_line
specifier|static
name|int
name|get_one_line
parameter_list|(
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|)
block|{
name|int
name|ret
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|len
operator|--
condition|)
block|{
name|char
name|c
init|=
operator|*
name|msg
operator|++
decl_stmt|;
name|ret
operator|++
expr_stmt|;
if|if
condition|(
name|c
operator|==
literal|'\n'
condition|)
break|break;
if|if
condition|(
operator|!
name|c
condition|)
return|return
literal|0
return|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|add_user_info
specifier|static
name|int
name|add_user_info
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
name|enum
name|cmit_fmt
name|fmt
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|char
modifier|*
name|date
decl_stmt|;
name|unsigned
name|int
name|namelen
decl_stmt|;
name|unsigned
name|long
name|time
decl_stmt|;
name|int
name|tz
decl_stmt|,
name|ret
decl_stmt|;
name|date
operator|=
name|strchr
argument_list|(
name|line
argument_list|,
literal|'>'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|date
condition|)
return|return
literal|0
return|;
name|namelen
operator|=
operator|++
name|date
operator|-
name|line
expr_stmt|;
name|time
operator|=
name|strtoul
argument_list|(
name|date
argument_list|,
operator|&
name|date
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|tz
operator|=
name|strtol
argument_list|(
name|date
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|ret
operator|=
name|sprintf
argument_list|(
name|buf
argument_list|,
literal|"%s: %.*s\n"
argument_list|,
name|what
argument_list|,
name|namelen
argument_list|,
name|line
argument_list|)
expr_stmt|;
if|if
condition|(
name|fmt
operator|==
name|CMIT_FMT_MEDIUM
condition|)
name|ret
operator|+=
name|sprintf
argument_list|(
name|buf
operator|+
name|ret
argument_list|,
literal|"Date:   %s\n"
argument_list|,
name|show_date
argument_list|(
name|time
argument_list|,
name|tz
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|is_empty_line
specifier|static
name|int
name|is_empty_line
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|)
block|{
while|while
condition|(
name|len
operator|&&
name|isspace
argument_list|(
name|line
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|len
operator|--
expr_stmt|;
return|return
operator|!
name|len
return|;
block|}
end_function
begin_function
DECL|function|add_parent_info
specifier|static
name|int
name|add_parent_info
parameter_list|(
name|enum
name|cmit_fmt
name|fmt
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|parents
parameter_list|)
block|{
name|int
name|offset
init|=
literal|0
decl_stmt|;
switch|switch
condition|(
name|parents
condition|)
block|{
case|case
literal|1
case|:
break|break;
case|case
literal|2
case|:
comment|/* Go back to the previous line: 40 characters of previous parent, and one '\n' */
name|offset
operator|=
name|sprintf
argument_list|(
name|buf
argument_list|,
literal|"Merge: %.40s\n"
argument_list|,
name|line
operator|-
literal|41
argument_list|)
expr_stmt|;
comment|/* Fallthrough */
default|default:
comment|/* Replace the previous '\n' with a space */
name|buf
index|[
name|offset
operator|-
literal|1
index|]
operator|=
literal|' '
expr_stmt|;
name|offset
operator|+=
name|sprintf
argument_list|(
name|buf
operator|+
name|offset
argument_list|,
literal|"%.40s\n"
argument_list|,
name|line
operator|+
literal|7
argument_list|)
expr_stmt|;
block|}
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|pretty_print_commit
name|unsigned
name|long
name|pretty_print_commit
parameter_list|(
name|enum
name|cmit_fmt
name|fmt
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|space
parameter_list|)
block|{
name|int
name|hdr
init|=
literal|1
decl_stmt|,
name|body
init|=
literal|0
decl_stmt|;
name|unsigned
name|long
name|offset
init|=
literal|0
decl_stmt|;
name|int
name|parents
init|=
literal|0
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|line
init|=
name|msg
decl_stmt|;
name|int
name|linelen
init|=
name|get_one_line
argument_list|(
name|msg
argument_list|,
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|linelen
condition|)
break|break;
comment|/* 		 * We want some slop for indentation and a possible 		 * final "...". Thus the "+ 20". 		 */
if|if
condition|(
name|offset
operator|+
name|linelen
operator|+
literal|20
operator|>
name|space
condition|)
block|{
name|memcpy
argument_list|(
name|buf
operator|+
name|offset
argument_list|,
literal|"    ...\n"
argument_list|,
literal|8
argument_list|)
expr_stmt|;
name|offset
operator|+=
literal|8
expr_stmt|;
break|break;
block|}
name|msg
operator|+=
name|linelen
expr_stmt|;
name|len
operator|-=
name|linelen
expr_stmt|;
if|if
condition|(
name|hdr
condition|)
block|{
if|if
condition|(
name|linelen
operator|==
literal|1
condition|)
block|{
name|hdr
operator|=
literal|0
expr_stmt|;
name|buf
index|[
name|offset
operator|++
index|]
operator|=
literal|'\n'
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|fmt
operator|==
name|CMIT_FMT_RAW
condition|)
block|{
name|memcpy
argument_list|(
name|buf
operator|+
name|offset
argument_list|,
name|line
argument_list|,
name|linelen
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|linelen
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
condition|)
block|{
if|if
condition|(
name|linelen
operator|!=
literal|48
condition|)
name|die
argument_list|(
literal|"bad parent line in commit"
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|add_parent_info
argument_list|(
name|fmt
argument_list|,
name|buf
operator|+
name|offset
argument_list|,
name|line
argument_list|,
operator|++
name|parents
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"author "
argument_list|,
literal|7
argument_list|)
condition|)
name|offset
operator|+=
name|add_user_info
argument_list|(
literal|"Author"
argument_list|,
name|fmt
argument_list|,
name|buf
operator|+
name|offset
argument_list|,
name|line
operator|+
literal|7
argument_list|)
expr_stmt|;
if|if
condition|(
name|fmt
operator|==
name|CMIT_FMT_FULL
condition|)
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|line
argument_list|,
literal|"committer "
argument_list|,
literal|10
argument_list|)
condition|)
name|offset
operator|+=
name|add_user_info
argument_list|(
literal|"Commit"
argument_list|,
name|fmt
argument_list|,
name|buf
operator|+
name|offset
argument_list|,
name|line
operator|+
literal|10
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
if|if
condition|(
name|is_empty_line
argument_list|(
name|line
argument_list|,
name|linelen
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|body
condition|)
continue|continue;
if|if
condition|(
name|fmt
operator|==
name|CMIT_FMT_SHORT
condition|)
break|break;
block|}
else|else
block|{
name|body
operator|=
literal|1
expr_stmt|;
block|}
name|memset
argument_list|(
name|buf
operator|+
name|offset
argument_list|,
literal|' '
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|buf
operator|+
name|offset
operator|+
literal|4
argument_list|,
name|line
argument_list|,
name|linelen
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|linelen
operator|+
literal|4
expr_stmt|;
block|}
comment|/* Make sure there is an EOLN */
if|if
condition|(
name|buf
index|[
name|offset
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|buf
index|[
name|offset
operator|++
index|]
operator|=
literal|'\n'
expr_stmt|;
name|buf
index|[
name|offset
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
name|offset
return|;
block|}
end_function
begin_function
DECL|function|pop_commit
name|struct
name|commit
modifier|*
name|pop_commit
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|stack
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|top
init|=
operator|*
name|stack
decl_stmt|;
name|struct
name|commit
modifier|*
name|item
init|=
name|top
condition|?
name|top
operator|->
name|item
else|:
name|NULL
decl_stmt|;
if|if
condition|(
name|top
condition|)
block|{
operator|*
name|stack
operator|=
name|top
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|top
argument_list|)
expr_stmt|;
block|}
return|return
name|item
return|;
block|}
end_function
begin_function
DECL|function|count_parents
name|int
name|count_parents
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|int
name|count
init|=
literal|0
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
for|for
control|(
name|count
operator|=
literal|0
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
operator|,
name|count
operator|++
control|)
empty_stmt|;
return|return
name|count
return|;
block|}
end_function
end_unit
