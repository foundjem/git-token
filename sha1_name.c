begin_unit
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
begin_function
DECL|function|find_short_object_filename
specifier|static
name|int
name|find_short_object_filename
parameter_list|(
name|int
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|alternate_object_database
modifier|*
name|alt
decl_stmt|;
name|char
name|hex
index|[
literal|40
index|]
decl_stmt|;
name|int
name|found
init|=
literal|0
decl_stmt|;
specifier|static
name|struct
name|alternate_object_database
modifier|*
name|fakeent
decl_stmt|;
if|if
condition|(
operator|!
name|fakeent
condition|)
block|{
specifier|const
name|char
modifier|*
name|objdir
init|=
name|get_object_directory
argument_list|()
decl_stmt|;
name|int
name|objdir_len
init|=
name|strlen
argument_list|(
name|objdir
argument_list|)
decl_stmt|;
name|int
name|entlen
init|=
name|objdir_len
operator|+
literal|43
decl_stmt|;
name|fakeent
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|fakeent
argument_list|)
operator|+
name|entlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|fakeent
operator|->
name|base
argument_list|,
name|objdir
argument_list|,
name|objdir_len
argument_list|)
expr_stmt|;
name|fakeent
operator|->
name|name
operator|=
name|fakeent
operator|->
name|base
operator|+
name|objdir_len
operator|+
literal|1
expr_stmt|;
name|fakeent
operator|->
name|name
index|[
operator|-
literal|1
index|]
operator|=
literal|'/'
expr_stmt|;
block|}
name|fakeent
operator|->
name|next
operator|=
name|alt_odb_list
expr_stmt|;
name|sprintf
argument_list|(
name|hex
argument_list|,
literal|"%.2s"
argument_list|,
name|name
argument_list|)
expr_stmt|;
for|for
control|(
name|alt
operator|=
name|fakeent
init|;
name|alt
operator|&&
name|found
operator|<
literal|2
condition|;
name|alt
operator|=
name|alt
operator|->
name|next
control|)
block|{
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|DIR
modifier|*
name|dir
decl_stmt|;
name|sprintf
argument_list|(
name|alt
operator|->
name|name
argument_list|,
literal|"%.2s/"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|dir
operator|=
name|opendir
argument_list|(
name|alt
operator|->
name|base
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
continue|continue;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|strlen
argument_list|(
name|de
operator|->
name|d_name
argument_list|)
operator|!=
literal|38
condition|)
continue|continue;
if|if
condition|(
name|memcmp
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
name|name
operator|+
literal|2
argument_list|,
name|len
operator|-
literal|2
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|found
condition|)
block|{
name|memcpy
argument_list|(
name|hex
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
expr_stmt|;
name|found
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|memcmp
argument_list|(
name|hex
operator|+
literal|2
argument_list|,
name|de
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
condition|)
block|{
name|found
operator|=
literal|2
expr_stmt|;
break|break;
block|}
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|found
operator|==
literal|1
condition|)
return|return
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|sha1
argument_list|)
operator|==
literal|0
return|;
return|return
name|found
return|;
block|}
end_function
begin_function
DECL|function|match_sha
specifier|static
name|int
name|match_sha
parameter_list|(
name|unsigned
name|len
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|a
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|b
parameter_list|)
block|{
do|do
block|{
if|if
condition|(
operator|*
name|a
operator|!=
operator|*
name|b
condition|)
return|return
literal|0
return|;
name|a
operator|++
expr_stmt|;
name|b
operator|++
expr_stmt|;
name|len
operator|-=
literal|2
expr_stmt|;
block|}
do|while
condition|(
name|len
operator|>
literal|1
condition|)
do|;
if|if
condition|(
name|len
condition|)
if|if
condition|(
operator|(
operator|*
name|a
operator|^
operator|*
name|b
operator|)
operator|&
literal|0xf0
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|find_short_packed_object
specifier|static
name|int
name|find_short_packed_object
parameter_list|(
name|int
name|len
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|match
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
name|unsigned
name|char
name|found_sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|found
init|=
literal|0
decl_stmt|;
name|prepare_packed_git
argument_list|()
expr_stmt|;
for|for
control|(
name|p
operator|=
name|packed_git
init|;
name|p
operator|&&
name|found
operator|<
literal|2
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
name|unsigned
name|num
init|=
name|num_packed_objects
argument_list|(
name|p
argument_list|)
decl_stmt|;
name|unsigned
name|first
init|=
literal|0
decl_stmt|,
name|last
init|=
name|num
decl_stmt|;
while|while
condition|(
name|first
operator|<
name|last
condition|)
block|{
name|unsigned
name|mid
init|=
operator|(
name|first
operator|+
name|last
operator|)
operator|/
literal|2
decl_stmt|;
name|unsigned
name|char
name|now
index|[
literal|20
index|]
decl_stmt|;
name|int
name|cmp
decl_stmt|;
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|mid
argument_list|,
name|now
argument_list|)
expr_stmt|;
name|cmp
operator|=
name|memcmp
argument_list|(
name|match
argument_list|,
name|now
argument_list|,
literal|20
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
block|{
name|first
operator|=
name|mid
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
block|{
name|first
operator|=
name|mid
operator|+
literal|1
expr_stmt|;
continue|continue;
block|}
name|last
operator|=
name|mid
expr_stmt|;
block|}
if|if
condition|(
name|first
operator|<
name|num
condition|)
block|{
name|unsigned
name|char
name|now
index|[
literal|20
index|]
decl_stmt|;
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|first
argument_list|,
name|now
argument_list|)
expr_stmt|;
if|if
condition|(
name|match_sha
argument_list|(
name|len
argument_list|,
name|match
argument_list|,
name|now
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|found
condition|)
block|{
name|memcpy
argument_list|(
name|found_sha1
argument_list|,
name|now
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|found
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|memcmp
argument_list|(
name|found_sha1
argument_list|,
name|now
argument_list|,
literal|20
argument_list|)
condition|)
block|{
name|found
operator|=
literal|2
expr_stmt|;
break|break;
block|}
block|}
block|}
block|}
if|if
condition|(
name|found
operator|==
literal|1
condition|)
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|found_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|found
return|;
block|}
end_function
begin_function
DECL|function|find_unique_short_object
specifier|static
name|int
name|find_unique_short_object
parameter_list|(
name|int
name|len
parameter_list|,
name|char
modifier|*
name|canonical
parameter_list|,
name|unsigned
name|char
modifier|*
name|res
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|has_unpacked
decl_stmt|,
name|has_packed
decl_stmt|;
name|unsigned
name|char
name|unpacked_sha1
index|[
literal|20
index|]
decl_stmt|,
name|packed_sha1
index|[
literal|20
index|]
decl_stmt|;
name|has_unpacked
operator|=
name|find_short_object_filename
argument_list|(
name|len
argument_list|,
name|canonical
argument_list|,
name|unpacked_sha1
argument_list|)
expr_stmt|;
name|has_packed
operator|=
name|find_short_packed_object
argument_list|(
name|len
argument_list|,
name|res
argument_list|,
name|packed_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|has_unpacked
operator|&&
operator|!
name|has_packed
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
literal|1
operator|<
name|has_unpacked
operator|||
literal|1
operator|<
name|has_packed
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|has_unpacked
operator|!=
name|has_packed
condition|)
block|{
name|memcpy
argument_list|(
name|sha1
argument_list|,
operator|(
name|has_packed
condition|?
name|packed_sha1
else|:
name|unpacked_sha1
operator|)
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* Both have unique ones -- do they match? */
if|if
condition|(
name|memcmp
argument_list|(
name|packed_sha1
argument_list|,
name|unpacked_sha1
argument_list|,
literal|20
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|packed_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_short_sha1
specifier|static
name|int
name|get_short_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|char
name|canonical
index|[
literal|40
index|]
decl_stmt|;
name|unsigned
name|char
name|res
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|4
condition|)
return|return
operator|-
literal|1
return|;
name|memset
argument_list|(
name|res
argument_list|,
literal|0
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|canonical
argument_list|,
literal|'x'
argument_list|,
literal|40
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
name|len
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|char
name|c
init|=
name|name
index|[
name|i
index|]
decl_stmt|;
name|unsigned
name|char
name|val
decl_stmt|;
if|if
condition|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
condition|)
name|val
operator|=
name|c
operator|-
literal|'0'
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'f'
condition|)
name|val
operator|=
name|c
operator|-
literal|'a'
operator|+
literal|10
expr_stmt|;
elseif|else
if|if
condition|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'F'
condition|)
block|{
name|val
operator|=
name|c
operator|-
literal|'A'
operator|+
literal|10
expr_stmt|;
name|c
operator|-=
literal|'A'
operator|-
literal|'a'
expr_stmt|;
block|}
else|else
return|return
operator|-
literal|1
return|;
name|canonical
index|[
name|i
index|]
operator|=
name|c
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|i
operator|&
literal|1
operator|)
condition|)
name|val
operator|<<=
literal|4
expr_stmt|;
name|res
index|[
name|i
operator|>>
literal|1
index|]
operator||=
name|val
expr_stmt|;
block|}
return|return
name|find_unique_short_object
argument_list|(
name|i
argument_list|,
name|canonical
argument_list|,
name|res
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|get_sha1_basic
specifier|static
name|int
name|get_sha1_basic
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
specifier|const
name|char
modifier|*
name|prefix
index|[]
init|=
block|{
literal|""
block|,
literal|"refs"
block|,
literal|"refs/tags"
block|,
literal|"refs/heads"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|len
operator|==
literal|40
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|str
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
for|for
control|(
name|p
operator|=
name|prefix
init|;
operator|*
name|p
condition|;
name|p
operator|++
control|)
block|{
name|char
modifier|*
name|pathname
init|=
name|git_path
argument_list|(
literal|"%s/%.*s"
argument_list|,
operator|*
name|p
argument_list|,
name|len
argument_list|,
name|str
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|read_ref
argument_list|(
name|pathname
argument_list|,
name|sha1
argument_list|)
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
begin_function_decl
specifier|static
name|int
name|get_sha1_1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|get_parent
specifier|static
name|int
name|get_parent
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|int
name|idx
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|ret
init|=
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
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
operator|-
literal|1
return|;
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|idx
condition|)
block|{
name|memcpy
argument_list|(
name|result
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|p
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
if|if
condition|(
operator|!
operator|--
name|idx
condition|)
block|{
name|memcpy
argument_list|(
name|result
argument_list|,
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|get_nth_ancestor
specifier|static
name|int
name|get_nth_ancestor
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|int
name|generation
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|ret
init|=
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
while|while
condition|(
name|generation
operator|--
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|commit
operator|||
name|parse_commit
argument_list|(
name|commit
argument_list|)
operator|||
operator|!
name|commit
operator|->
name|parents
condition|)
return|return
operator|-
literal|1
return|;
name|memcpy
argument_list|(
name|sha1
argument_list|,
name|commit
operator|->
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|result
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_sha1_1
specifier|static
name|int
name|get_sha1_1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|parent
decl_stmt|,
name|ret
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
comment|/* foo^[0-9] or foo^ (== foo^1); we do not do more than 9 parents. */
if|if
condition|(
name|len
operator|>
literal|2
operator|&&
name|name
index|[
name|len
operator|-
literal|2
index|]
operator|==
literal|'^'
operator|&&
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|>=
literal|'0'
operator|&&
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|<=
literal|'9'
condition|)
block|{
name|parent
operator|=
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|-
literal|'0'
expr_stmt|;
name|len
operator|-=
literal|2
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|len
operator|>
literal|1
operator|&&
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'^'
condition|)
block|{
name|parent
operator|=
literal|1
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
else|else
name|parent
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|parent
operator|>=
literal|0
condition|)
return|return
name|get_parent
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|,
name|parent
argument_list|)
return|;
comment|/* "name~3" is "name^^^", 	 * "name~12" is "name^^^^^^^^^^^^", and 	 * "name~" and "name~0" are name -- not "name^0"! 	 */
name|parent
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|name
operator|+
name|len
operator|-
literal|1
init|;
name|name
operator|<=
name|cp
condition|;
name|cp
operator|--
control|)
block|{
name|int
name|ch
init|=
operator|*
name|cp
decl_stmt|;
if|if
condition|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'9'
condition|)
continue|continue;
if|if
condition|(
name|ch
operator|!=
literal|'~'
condition|)
name|parent
operator|=
operator|-
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|parent
operator|&&
operator|*
name|cp
operator|==
literal|'~'
condition|)
block|{
name|int
name|len1
init|=
name|cp
operator|-
name|name
decl_stmt|;
name|cp
operator|++
expr_stmt|;
while|while
condition|(
name|cp
operator|<
name|name
operator|+
name|len
condition|)
name|parent
operator|=
name|parent
operator|*
literal|10
operator|+
operator|*
name|cp
operator|++
operator|-
literal|'0'
expr_stmt|;
return|return
name|get_nth_ancestor
argument_list|(
name|name
argument_list|,
name|len1
argument_list|,
name|sha1
argument_list|,
name|parent
argument_list|)
return|;
block|}
name|ret
operator|=
name|get_sha1_basic
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
return|return
literal|0
return|;
return|return
name|get_short_sha1
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * This is like "get_sha1_basic()", except it allows "sha1 expressions",  * notably "xyz^" for "parent of xyz"  */
end_comment
begin_function
DECL|function|get_sha1
name|int
name|get_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|prepare_alt_odb
argument_list|()
expr_stmt|;
return|return
name|get_sha1_1
argument_list|(
name|name
argument_list|,
name|strlen
argument_list|(
name|name
argument_list|)
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
end_unit
