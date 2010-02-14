begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_function
DECL|function|get_mode
specifier|static
specifier|const
name|char
modifier|*
name|get_mode
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|unsigned
name|int
modifier|*
name|modep
parameter_list|)
block|{
name|unsigned
name|char
name|c
decl_stmt|;
name|unsigned
name|int
name|mode
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|*
name|str
operator|==
literal|' '
condition|)
return|return
name|NULL
return|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|str
operator|++
operator|)
operator|!=
literal|' '
condition|)
block|{
if|if
condition|(
name|c
operator|<
literal|'0'
operator|||
name|c
operator|>
literal|'7'
condition|)
return|return
name|NULL
return|;
name|mode
operator|=
operator|(
name|mode
operator|<<
literal|3
operator|)
operator|+
operator|(
name|c
operator|-
literal|'0'
operator|)
expr_stmt|;
block|}
operator|*
name|modep
operator|=
name|mode
expr_stmt|;
return|return
name|str
return|;
block|}
end_function
begin_function
DECL|function|decode_tree_entry
specifier|static
name|void
name|decode_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|,
name|len
decl_stmt|;
if|if
condition|(
name|size
operator|<
literal|24
operator|||
name|buf
index|[
name|size
operator|-
literal|21
index|]
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
name|path
operator|=
name|get_mode
argument_list|(
name|buf
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|path
operator|||
operator|!
operator|*
name|path
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
operator|+
literal|1
expr_stmt|;
comment|/* Initialize the descriptor entry */
name|desc
operator|->
name|entry
operator|.
name|path
operator|=
name|path
expr_stmt|;
name|desc
operator|->
name|entry
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
name|desc
operator|->
name|entry
operator|.
name|sha1
operator|=
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
operator|(
name|path
operator|+
name|len
operator|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|init_tree_desc
name|void
name|init_tree_desc
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|desc
operator|->
name|buffer
operator|=
name|buffer
expr_stmt|;
name|desc
operator|->
name|size
operator|=
name|size
expr_stmt|;
if|if
condition|(
name|size
condition|)
name|decode_tree_entry
argument_list|(
name|desc
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fill_tree_descriptor
name|void
modifier|*
name|fill_tree_descriptor
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|long
name|size
init|=
literal|0
decl_stmt|;
name|void
modifier|*
name|buf
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|sha1
condition|)
block|{
name|buf
operator|=
name|read_object_with_reference
argument_list|(
name|sha1
argument_list|,
name|tree_type
argument_list|,
operator|&
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
name|die
argument_list|(
literal|"unable to read tree %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|init_tree_desc
argument_list|(
name|desc
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_function
DECL|function|entry_compare
specifier|static
name|int
name|entry_compare
parameter_list|(
name|struct
name|name_entry
modifier|*
name|a
parameter_list|,
name|struct
name|name_entry
modifier|*
name|b
parameter_list|)
block|{
return|return
name|df_name_compare
argument_list|(
name|a
operator|->
name|path
argument_list|,
name|tree_entry_len
argument_list|(
name|a
operator|->
name|path
argument_list|,
name|a
operator|->
name|sha1
argument_list|)
argument_list|,
name|a
operator|->
name|mode
argument_list|,
name|b
operator|->
name|path
argument_list|,
name|tree_entry_len
argument_list|(
name|b
operator|->
name|path
argument_list|,
name|b
operator|->
name|sha1
argument_list|)
argument_list|,
name|b
operator|->
name|mode
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|entry_clear
specifier|static
name|void
name|entry_clear
parameter_list|(
name|struct
name|name_entry
modifier|*
name|a
parameter_list|)
block|{
name|memset
argument_list|(
name|a
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|a
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|entry_extract
specifier|static
name|void
name|entry_extract
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
name|struct
name|name_entry
modifier|*
name|a
parameter_list|)
block|{
operator|*
name|a
operator|=
name|t
operator|->
name|entry
expr_stmt|;
block|}
end_function
begin_function
DECL|function|update_tree_entry
name|void
name|update_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|)
block|{
specifier|const
name|void
modifier|*
name|buf
init|=
name|desc
operator|->
name|buffer
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|end
init|=
name|desc
operator|->
name|entry
operator|.
name|sha1
operator|+
literal|20
decl_stmt|;
name|unsigned
name|long
name|size
init|=
name|desc
operator|->
name|size
decl_stmt|;
name|unsigned
name|long
name|len
init|=
name|end
operator|-
operator|(
specifier|const
name|unsigned
name|char
operator|*
operator|)
name|buf
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|len
condition|)
name|die
argument_list|(
literal|"corrupt tree file"
argument_list|)
expr_stmt|;
name|buf
operator|=
name|end
expr_stmt|;
name|size
operator|-=
name|len
expr_stmt|;
name|desc
operator|->
name|buffer
operator|=
name|buf
expr_stmt|;
name|desc
operator|->
name|size
operator|=
name|size
expr_stmt|;
if|if
condition|(
name|size
condition|)
name|decode_tree_entry
argument_list|(
name|desc
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|tree_entry
name|int
name|tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
name|struct
name|name_entry
modifier|*
name|entry
parameter_list|)
block|{
if|if
condition|(
operator|!
name|desc
operator|->
name|size
condition|)
return|return
literal|0
return|;
operator|*
name|entry
operator|=
name|desc
operator|->
name|entry
expr_stmt|;
name|update_tree_entry
argument_list|(
name|desc
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|setup_traverse_info
name|void
name|setup_traverse_info
parameter_list|(
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|int
name|pathlen
init|=
name|strlen
argument_list|(
name|base
argument_list|)
decl_stmt|;
specifier|static
name|struct
name|traverse_info
name|dummy
decl_stmt|;
name|memset
argument_list|(
name|info
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|info
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathlen
operator|&&
name|base
index|[
name|pathlen
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|pathlen
operator|--
expr_stmt|;
name|info
operator|->
name|pathlen
operator|=
name|pathlen
condition|?
name|pathlen
operator|+
literal|1
else|:
literal|0
expr_stmt|;
name|info
operator|->
name|name
operator|.
name|path
operator|=
name|base
expr_stmt|;
name|info
operator|->
name|name
operator|.
name|sha1
operator|=
operator|(
name|void
operator|*
operator|)
operator|(
name|base
operator|+
name|pathlen
operator|+
literal|1
operator|)
expr_stmt|;
if|if
condition|(
name|pathlen
condition|)
name|info
operator|->
name|prev
operator|=
operator|&
name|dummy
expr_stmt|;
block|}
end_function
begin_function
DECL|function|make_traverse_path
name|char
modifier|*
name|make_traverse_path
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
specifier|const
name|struct
name|name_entry
modifier|*
name|n
parameter_list|)
block|{
name|int
name|len
init|=
name|tree_entry_len
argument_list|(
name|n
operator|->
name|path
argument_list|,
name|n
operator|->
name|sha1
argument_list|)
decl_stmt|;
name|int
name|pathlen
init|=
name|info
operator|->
name|pathlen
decl_stmt|;
name|path
index|[
name|pathlen
operator|+
name|len
index|]
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|memcpy
argument_list|(
name|path
operator|+
name|pathlen
argument_list|,
name|n
operator|->
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pathlen
condition|)
break|break;
name|path
index|[
operator|--
name|pathlen
index|]
operator|=
literal|'/'
expr_stmt|;
name|n
operator|=
operator|&
name|info
operator|->
name|name
expr_stmt|;
name|len
operator|=
name|tree_entry_len
argument_list|(
name|n
operator|->
name|path
argument_list|,
name|n
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|info
operator|=
name|info
operator|->
name|prev
expr_stmt|;
name|pathlen
operator|-=
name|len
expr_stmt|;
block|}
return|return
name|path
return|;
block|}
end_function
begin_function
DECL|function|traverse_trees
name|int
name|traverse_trees
parameter_list|(
name|int
name|n
parameter_list|,
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|)
block|{
name|int
name|ret
init|=
literal|0
decl_stmt|;
name|struct
name|name_entry
modifier|*
name|entry
init|=
name|xmalloc
argument_list|(
name|n
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|entry
argument_list|)
argument_list|)
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|long
name|mask
init|=
literal|0
decl_stmt|;
name|unsigned
name|long
name|dirmask
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|,
name|last
decl_stmt|;
name|last
operator|=
operator|-
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|t
index|[
name|i
index|]
operator|.
name|size
condition|)
continue|continue;
name|entry_extract
argument_list|(
name|t
operator|+
name|i
argument_list|,
name|entry
operator|+
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
name|last
operator|>=
literal|0
condition|)
block|{
name|int
name|cmp
init|=
name|entry_compare
argument_list|(
name|entry
operator|+
name|i
argument_list|,
name|entry
operator|+
name|last
argument_list|)
decl_stmt|;
comment|/* 				 * Is the new name bigger than the old one? 				 * Ignore it 				 */
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
continue|continue;
comment|/* 				 * Is the new name smaller than the old one? 				 * Ignore all old ones 				 */
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
name|mask
operator|=
literal|0
expr_stmt|;
block|}
name|mask
operator||=
literal|1ul
operator|<<
name|i
expr_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|entry
index|[
name|i
index|]
operator|.
name|mode
argument_list|)
condition|)
name|dirmask
operator||=
literal|1ul
operator|<<
name|i
expr_stmt|;
name|last
operator|=
name|i
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|mask
condition|)
break|break;
name|dirmask
operator|&=
name|mask
expr_stmt|;
comment|/* 		 * Clear all the unused name-entries. 		 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|mask
operator|&
operator|(
literal|1ul
operator|<<
name|i
operator|)
condition|)
continue|continue;
name|entry_clear
argument_list|(
name|entry
operator|+
name|i
argument_list|)
expr_stmt|;
block|}
name|ret
operator|=
name|info
operator|->
name|fn
argument_list|(
name|n
argument_list|,
name|mask
argument_list|,
name|dirmask
argument_list|,
name|entry
argument_list|,
name|info
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
break|break;
if|if
condition|(
name|ret
condition|)
name|mask
operator|&=
name|ret
expr_stmt|;
name|ret
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|n
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|mask
operator|&
operator|(
literal|1ul
operator|<<
name|i
operator|)
condition|)
name|update_tree_entry
argument_list|(
name|t
operator|+
name|i
argument_list|)
expr_stmt|;
block|}
block|}
name|free
argument_list|(
name|entry
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|find_tree_entry
specifier|static
name|int
name|find_tree_entry
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|unsigned
modifier|*
name|mode
parameter_list|)
block|{
name|int
name|namelen
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
while|while
condition|(
name|t
operator|->
name|size
condition|)
block|{
specifier|const
name|char
modifier|*
name|entry
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|int
name|entrylen
decl_stmt|,
name|cmp
decl_stmt|;
name|sha1
operator|=
name|tree_entry_extract
argument_list|(
name|t
argument_list|,
operator|&
name|entry
argument_list|,
name|mode
argument_list|)
expr_stmt|;
name|update_tree_entry
argument_list|(
name|t
argument_list|)
expr_stmt|;
name|entrylen
operator|=
name|tree_entry_len
argument_list|(
name|entry
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|entrylen
operator|>
name|namelen
condition|)
continue|continue;
name|cmp
operator|=
name|memcmp
argument_list|(
name|name
argument_list|,
name|entry
argument_list|,
name|entrylen
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
continue|continue;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
break|break;
if|if
condition|(
name|entrylen
operator|==
name|namelen
condition|)
block|{
name|hashcpy
argument_list|(
name|result
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|name
index|[
name|entrylen
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
operator|*
name|mode
argument_list|)
condition|)
break|break;
if|if
condition|(
operator|++
name|entrylen
operator|==
name|namelen
condition|)
block|{
name|hashcpy
argument_list|(
name|result
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|get_tree_entry
argument_list|(
name|sha1
argument_list|,
name|name
operator|+
name|entrylen
argument_list|,
name|result
argument_list|,
name|mode
argument_list|)
return|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|get_tree_entry
name|int
name|get_tree_entry
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|tree_sha1
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
parameter_list|,
name|unsigned
modifier|*
name|mode
parameter_list|)
block|{
name|int
name|retval
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|struct
name|tree_desc
name|t
decl_stmt|;
name|unsigned
name|char
name|root
index|[
literal|20
index|]
decl_stmt|;
name|tree
operator|=
name|read_object_with_reference
argument_list|(
name|tree_sha1
argument_list|,
name|tree_type
argument_list|,
operator|&
name|size
argument_list|,
name|root
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|name
index|[
literal|0
index|]
operator|==
literal|'\0'
condition|)
block|{
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|root
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|init_tree_desc
argument_list|(
operator|&
name|t
argument_list|,
name|tree
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|retval
operator|=
name|find_tree_entry
argument_list|(
operator|&
name|t
argument_list|,
name|name
argument_list|,
name|sha1
argument_list|,
name|mode
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
end_unit
