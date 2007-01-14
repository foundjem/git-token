begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|DEBUG
end_ifndef
begin_define
DECL|macro|DEBUG
define|#
directive|define
name|DEBUG
value|0
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|cache_tree
name|struct
name|cache_tree
modifier|*
name|cache_tree
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|cache_tree
modifier|*
name|it
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|cache_tree
argument_list|)
argument_list|)
decl_stmt|;
name|it
operator|->
name|entry_count
operator|=
operator|-
literal|1
expr_stmt|;
return|return
name|it
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_free
name|void
name|cache_tree_free
parameter_list|(
name|struct
name|cache_tree
modifier|*
modifier|*
name|it_p
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|cache_tree
modifier|*
name|it
init|=
operator|*
name|it_p
decl_stmt|;
if|if
condition|(
operator|!
name|it
condition|)
return|return;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|it
operator|->
name|subtree_nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|it
operator|->
name|down
index|[
name|i
index|]
condition|)
name|cache_tree_free
argument_list|(
operator|&
name|it
operator|->
name|down
index|[
name|i
index|]
operator|->
name|cache_tree
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|it
operator|->
name|down
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|it
argument_list|)
expr_stmt|;
operator|*
name|it_p
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|subtree_name_cmp
specifier|static
name|int
name|subtree_name_cmp
parameter_list|(
specifier|const
name|char
modifier|*
name|one
parameter_list|,
name|int
name|onelen
parameter_list|,
specifier|const
name|char
modifier|*
name|two
parameter_list|,
name|int
name|twolen
parameter_list|)
block|{
if|if
condition|(
name|onelen
operator|<
name|twolen
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|twolen
operator|<
name|onelen
condition|)
return|return
literal|1
return|;
return|return
name|memcmp
argument_list|(
name|one
argument_list|,
name|two
argument_list|,
name|onelen
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|subtree_pos
specifier|static
name|int
name|subtree_pos
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|)
block|{
name|struct
name|cache_tree_sub
modifier|*
modifier|*
name|down
init|=
name|it
operator|->
name|down
decl_stmt|;
name|int
name|lo
decl_stmt|,
name|hi
decl_stmt|;
name|lo
operator|=
literal|0
expr_stmt|;
name|hi
operator|=
name|it
operator|->
name|subtree_nr
expr_stmt|;
while|while
condition|(
name|lo
operator|<
name|hi
condition|)
block|{
name|int
name|mi
init|=
operator|(
name|lo
operator|+
name|hi
operator|)
operator|/
literal|2
decl_stmt|;
name|struct
name|cache_tree_sub
modifier|*
name|mdl
init|=
name|down
index|[
name|mi
index|]
decl_stmt|;
name|int
name|cmp
init|=
name|subtree_name_cmp
argument_list|(
name|path
argument_list|,
name|pathlen
argument_list|,
name|mdl
operator|->
name|name
argument_list|,
name|mdl
operator|->
name|namelen
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|cmp
condition|)
return|return
name|mi
return|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
name|hi
operator|=
name|mi
expr_stmt|;
else|else
name|lo
operator|=
name|mi
operator|+
literal|1
expr_stmt|;
block|}
return|return
operator|-
name|lo
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|find_subtree
specifier|static
name|struct
name|cache_tree_sub
modifier|*
name|find_subtree
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|,
name|int
name|create
parameter_list|)
block|{
name|struct
name|cache_tree_sub
modifier|*
name|down
decl_stmt|;
name|int
name|pos
init|=
name|subtree_pos
argument_list|(
name|it
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
return|return
name|it
operator|->
name|down
index|[
name|pos
index|]
return|;
if|if
condition|(
operator|!
name|create
condition|)
return|return
name|NULL
return|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|it
operator|->
name|subtree_alloc
operator|<=
name|it
operator|->
name|subtree_nr
condition|)
block|{
name|it
operator|->
name|subtree_alloc
operator|=
name|alloc_nr
argument_list|(
name|it
operator|->
name|subtree_alloc
argument_list|)
expr_stmt|;
name|it
operator|->
name|down
operator|=
name|xrealloc
argument_list|(
name|it
operator|->
name|down
argument_list|,
name|it
operator|->
name|subtree_alloc
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|it
operator|->
name|down
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|it
operator|->
name|subtree_nr
operator|++
expr_stmt|;
name|down
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|down
argument_list|)
operator|+
name|pathlen
operator|+
literal|1
argument_list|)
expr_stmt|;
name|down
operator|->
name|cache_tree
operator|=
name|NULL
expr_stmt|;
name|down
operator|->
name|namelen
operator|=
name|pathlen
expr_stmt|;
name|memcpy
argument_list|(
name|down
operator|->
name|name
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
name|down
operator|->
name|name
index|[
name|pathlen
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|pos
operator|<
name|it
operator|->
name|subtree_nr
condition|)
name|memmove
argument_list|(
name|it
operator|->
name|down
operator|+
name|pos
operator|+
literal|1
argument_list|,
name|it
operator|->
name|down
operator|+
name|pos
argument_list|,
sizeof|sizeof
argument_list|(
name|down
argument_list|)
operator|*
operator|(
name|it
operator|->
name|subtree_nr
operator|-
name|pos
operator|-
literal|1
operator|)
argument_list|)
expr_stmt|;
name|it
operator|->
name|down
index|[
name|pos
index|]
operator|=
name|down
expr_stmt|;
return|return
name|down
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_sub
name|struct
name|cache_tree_sub
modifier|*
name|cache_tree_sub
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|pathlen
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
return|return
name|find_subtree
argument_list|(
name|it
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_invalidate_path
name|void
name|cache_tree_invalidate_path
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
comment|/* a/b/c 	 * ==> invalidate self 	 * ==> find "a", have it invalidate "b/c" 	 * a 	 * ==> invalidate self 	 * ==> if "a" exists as a subtree, remove it. 	 */
specifier|const
name|char
modifier|*
name|slash
decl_stmt|;
name|int
name|namelen
decl_stmt|;
name|struct
name|cache_tree_sub
modifier|*
name|down
decl_stmt|;
if|#
directive|if
name|DEBUG
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree invalidate<%s>\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
operator|!
name|it
condition|)
return|return;
name|slash
operator|=
name|strchr
argument_list|(
name|path
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|it
operator|->
name|entry_count
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|slash
condition|)
block|{
name|int
name|pos
decl_stmt|;
name|namelen
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|pos
operator|=
name|subtree_pos
argument_list|(
name|it
argument_list|,
name|path
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
block|{
name|cache_tree_free
argument_list|(
operator|&
name|it
operator|->
name|down
index|[
name|pos
index|]
operator|->
name|cache_tree
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|it
operator|->
name|down
index|[
name|pos
index|]
argument_list|)
expr_stmt|;
comment|/* 0 1 2 3 4 5 			 *       ^     ^subtree_nr = 6 			 *       pos 			 * move 4 and 5 up one place (2 entries) 			 * 2 = 6 - 3 - 1 = subtree_nr - pos - 1 			 */
name|memmove
argument_list|(
name|it
operator|->
name|down
operator|+
name|pos
argument_list|,
name|it
operator|->
name|down
operator|+
name|pos
operator|+
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|cache_tree_sub
operator|*
argument_list|)
operator|*
operator|(
name|it
operator|->
name|subtree_nr
operator|-
name|pos
operator|-
literal|1
operator|)
argument_list|)
expr_stmt|;
name|it
operator|->
name|subtree_nr
operator|--
expr_stmt|;
block|}
return|return;
block|}
name|namelen
operator|=
name|slash
operator|-
name|path
expr_stmt|;
name|down
operator|=
name|find_subtree
argument_list|(
name|it
argument_list|,
name|path
argument_list|,
name|namelen
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|down
condition|)
name|cache_tree_invalidate_path
argument_list|(
name|down
operator|->
name|cache_tree
argument_list|,
name|slash
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|verify_cache
specifier|static
name|int
name|verify_cache
parameter_list|(
name|struct
name|cache_entry
modifier|*
modifier|*
name|cache
parameter_list|,
name|int
name|entries
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|funny
decl_stmt|;
comment|/* Verify that the tree is merged */
name|funny
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
name|entries
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
if|if
condition|(
literal|10
operator|<
operator|++
name|funny
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"...\n"
argument_list|)
expr_stmt|;
break|break;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: unmerged (%s)\n"
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|funny
condition|)
return|return
operator|-
literal|1
return|;
comment|/* Also verify that the cache does not have path and path/file 	 * at the same time.  At this point we know the cache has only 	 * stage 0 entries. 	 */
name|funny
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
name|entries
operator|-
literal|1
condition|;
name|i
operator|++
control|)
block|{
comment|/* path/file always comes after path because of the way 		 * the cache is sorted.  Also path can appear only once, 		 * which means conflicting one would immediately follow. 		 */
specifier|const
name|char
modifier|*
name|this_name
init|=
name|cache
index|[
name|i
index|]
operator|->
name|name
decl_stmt|;
specifier|const
name|char
modifier|*
name|next_name
init|=
name|cache
index|[
name|i
operator|+
literal|1
index|]
operator|->
name|name
decl_stmt|;
name|int
name|this_len
init|=
name|strlen
argument_list|(
name|this_name
argument_list|)
decl_stmt|;
if|if
condition|(
name|this_len
operator|<
name|strlen
argument_list|(
name|next_name
argument_list|)
operator|&&
name|strncmp
argument_list|(
name|this_name
argument_list|,
name|next_name
argument_list|,
name|this_len
argument_list|)
operator|==
literal|0
operator|&&
name|next_name
index|[
name|this_len
index|]
operator|==
literal|'/'
condition|)
block|{
if|if
condition|(
literal|10
operator|<
operator|++
name|funny
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"...\n"
argument_list|)
expr_stmt|;
break|break;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"You have both %s and %s\n"
argument_list|,
name|this_name
argument_list|,
name|next_name
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|funny
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|discard_unused_subtrees
specifier|static
name|void
name|discard_unused_subtrees
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|)
block|{
name|struct
name|cache_tree_sub
modifier|*
modifier|*
name|down
init|=
name|it
operator|->
name|down
decl_stmt|;
name|int
name|nr
init|=
name|it
operator|->
name|subtree_nr
decl_stmt|;
name|int
name|dst
decl_stmt|,
name|src
decl_stmt|;
for|for
control|(
name|dst
operator|=
name|src
operator|=
literal|0
init|;
name|src
operator|<
name|nr
condition|;
name|src
operator|++
control|)
block|{
name|struct
name|cache_tree_sub
modifier|*
name|s
init|=
name|down
index|[
name|src
index|]
decl_stmt|;
if|if
condition|(
name|s
operator|->
name|used
condition|)
name|down
index|[
name|dst
operator|++
index|]
operator|=
name|s
expr_stmt|;
else|else
block|{
name|cache_tree_free
argument_list|(
operator|&
name|s
operator|->
name|cache_tree
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|s
argument_list|)
expr_stmt|;
name|it
operator|->
name|subtree_nr
operator|--
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|cache_tree_fully_valid
name|int
name|cache_tree_fully_valid
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|it
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|it
operator|->
name|entry_count
operator|<
literal|0
operator|||
operator|!
name|has_sha1_file
argument_list|(
name|it
operator|->
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|it
operator|->
name|subtree_nr
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|cache_tree_fully_valid
argument_list|(
name|it
operator|->
name|down
index|[
name|i
index|]
operator|->
name|cache_tree
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|update_one
specifier|static
name|int
name|update_one
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
name|cache
parameter_list|,
name|int
name|entries
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|int
name|missing_ok
parameter_list|,
name|int
name|dryrun
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|,
name|offset
decl_stmt|;
name|char
modifier|*
name|buffer
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
name|it
operator|->
name|entry_count
operator|&&
name|has_sha1_file
argument_list|(
name|it
operator|->
name|sha1
argument_list|)
condition|)
return|return
name|it
operator|->
name|entry_count
return|;
comment|/* 	 * We first scan for subtrees and update them; we start by 	 * marking existing subtrees -- the ones that are unmarked 	 * should not be in the result. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|it
operator|->
name|subtree_nr
condition|;
name|i
operator|++
control|)
name|it
operator|->
name|down
index|[
name|i
index|]
operator|->
name|used
operator|=
literal|0
expr_stmt|;
comment|/* 	 * Find the subtrees and update them. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|entries
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|cache
index|[
name|i
index|]
decl_stmt|;
name|struct
name|cache_tree_sub
modifier|*
name|sub
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|,
modifier|*
name|slash
decl_stmt|;
name|int
name|pathlen
decl_stmt|,
name|sublen
decl_stmt|,
name|subcnt
decl_stmt|;
name|path
operator|=
name|ce
operator|->
name|name
expr_stmt|;
name|pathlen
operator|=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathlen
operator|<=
name|baselen
operator|||
name|memcmp
argument_list|(
name|base
argument_list|,
name|path
argument_list|,
name|baselen
argument_list|)
condition|)
break|break;
comment|/* at the end of this level */
name|slash
operator|=
name|strchr
argument_list|(
name|path
operator|+
name|baselen
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|slash
condition|)
continue|continue;
comment|/* 		 * a/bbb/c (base = a/, slash = /c) 		 * ==> 		 * path+baselen = bbb/c, sublen = 3 		 */
name|sublen
operator|=
name|slash
operator|-
operator|(
name|path
operator|+
name|baselen
operator|)
expr_stmt|;
name|sub
operator|=
name|find_subtree
argument_list|(
name|it
argument_list|,
name|path
operator|+
name|baselen
argument_list|,
name|sublen
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sub
operator|->
name|cache_tree
condition|)
name|sub
operator|->
name|cache_tree
operator|=
name|cache_tree
argument_list|()
expr_stmt|;
name|subcnt
operator|=
name|update_one
argument_list|(
name|sub
operator|->
name|cache_tree
argument_list|,
name|cache
operator|+
name|i
argument_list|,
name|entries
operator|-
name|i
argument_list|,
name|path
argument_list|,
name|baselen
operator|+
name|sublen
operator|+
literal|1
argument_list|,
name|missing_ok
argument_list|,
name|dryrun
argument_list|)
expr_stmt|;
if|if
condition|(
name|subcnt
operator|<
literal|0
condition|)
return|return
name|subcnt
return|;
name|i
operator|+=
name|subcnt
operator|-
literal|1
expr_stmt|;
name|sub
operator|->
name|used
operator|=
literal|1
expr_stmt|;
block|}
name|discard_unused_subtrees
argument_list|(
name|it
argument_list|)
expr_stmt|;
comment|/* 	 * Then write out the tree object for this level. 	 */
name|size
operator|=
literal|8192
expr_stmt|;
name|buffer
operator|=
name|xmalloc
argument_list|(
name|size
argument_list|)
expr_stmt|;
name|offset
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
name|entries
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|cache
index|[
name|i
index|]
decl_stmt|;
name|struct
name|cache_tree_sub
modifier|*
name|sub
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|,
modifier|*
name|slash
decl_stmt|;
name|int
name|pathlen
decl_stmt|,
name|entlen
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|unsigned
name|mode
decl_stmt|;
name|path
operator|=
name|ce
operator|->
name|name
expr_stmt|;
name|pathlen
operator|=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathlen
operator|<=
name|baselen
operator|||
name|memcmp
argument_list|(
name|base
argument_list|,
name|path
argument_list|,
name|baselen
argument_list|)
condition|)
break|break;
comment|/* at the end of this level */
name|slash
operator|=
name|strchr
argument_list|(
name|path
operator|+
name|baselen
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
name|slash
condition|)
block|{
name|entlen
operator|=
name|slash
operator|-
operator|(
name|path
operator|+
name|baselen
operator|)
expr_stmt|;
name|sub
operator|=
name|find_subtree
argument_list|(
name|it
argument_list|,
name|path
operator|+
name|baselen
argument_list|,
name|entlen
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sub
condition|)
name|die
argument_list|(
literal|"cache-tree.c: '%.*s' in '%s' not found"
argument_list|,
name|entlen
argument_list|,
name|path
operator|+
name|baselen
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|i
operator|+=
name|sub
operator|->
name|cache_tree
operator|->
name|entry_count
operator|-
literal|1
expr_stmt|;
name|sha1
operator|=
name|sub
operator|->
name|cache_tree
operator|->
name|sha1
expr_stmt|;
name|mode
operator|=
name|S_IFDIR
expr_stmt|;
block|}
else|else
block|{
name|sha1
operator|=
name|ce
operator|->
name|sha1
expr_stmt|;
name|mode
operator|=
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|entlen
operator|=
name|pathlen
operator|-
name|baselen
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|missing_ok
operator|&&
operator|!
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"invalid object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
operator|!
name|ce
operator|->
name|ce_mode
condition|)
continue|continue;
comment|/* entry being removed */
if|if
condition|(
name|size
operator|<
name|offset
operator|+
name|entlen
operator|+
literal|100
condition|)
block|{
name|size
operator|=
name|alloc_nr
argument_list|(
name|offset
operator|+
name|entlen
operator|+
literal|100
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
name|offset
operator|+=
name|sprintf
argument_list|(
name|buffer
operator|+
name|offset
argument_list|,
literal|"%o %.*s"
argument_list|,
name|mode
argument_list|,
name|entlen
argument_list|,
name|path
operator|+
name|baselen
argument_list|)
expr_stmt|;
name|buffer
index|[
name|offset
operator|++
index|]
operator|=
literal|0
expr_stmt|;
name|hashcpy
argument_list|(
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buffer
operator|+
name|offset
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|offset
operator|+=
literal|20
expr_stmt|;
if|#
directive|if
name|DEBUG
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree update-one %o %.*s\n"
argument_list|,
name|mode
argument_list|,
name|entlen
argument_list|,
name|path
operator|+
name|baselen
argument_list|)
expr_stmt|;
endif|#
directive|endif
block|}
if|if
condition|(
name|dryrun
condition|)
name|hash_sha1_file
argument_list|(
name|buffer
argument_list|,
name|offset
argument_list|,
name|tree_type
argument_list|,
name|it
operator|->
name|sha1
argument_list|)
expr_stmt|;
else|else
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|offset
argument_list|,
name|tree_type
argument_list|,
name|it
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|it
operator|->
name|entry_count
operator|=
name|i
expr_stmt|;
if|#
directive|if
name|DEBUG
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree update-one (%d ent, %d subtree) %s\n"
argument_list|,
name|it
operator|->
name|entry_count
argument_list|,
name|it
operator|->
name|subtree_nr
argument_list|,
name|sha1_to_hex
argument_list|(
name|it
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
endif|#
directive|endif
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_update
name|int
name|cache_tree_update
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
name|cache
parameter_list|,
name|int
name|entries
parameter_list|,
name|int
name|missing_ok
parameter_list|,
name|int
name|dryrun
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|i
operator|=
name|verify_cache
argument_list|(
name|cache
argument_list|,
name|entries
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
condition|)
return|return
name|i
return|;
name|i
operator|=
name|update_one
argument_list|(
name|it
argument_list|,
name|cache
argument_list|,
name|entries
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|missing_ok
argument_list|,
name|dryrun
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|<
literal|0
condition|)
return|return
name|i
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_one
specifier|static
name|void
modifier|*
name|write_one
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
name|char
modifier|*
name|path
parameter_list|,
name|int
name|pathlen
parameter_list|,
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|,
name|unsigned
name|long
modifier|*
name|offset
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* One "cache-tree" entry consists of the following: 	 * path (NUL terminated) 	 * entry_count, subtree_nr ("%d %d\n") 	 * tree-sha1 (missing if invalid) 	 * subtree_nr "cache-tree" entries for subtrees. 	 */
if|if
condition|(
operator|*
name|size
operator|<
operator|*
name|offset
operator|+
name|pathlen
operator|+
literal|100
condition|)
block|{
operator|*
name|size
operator|=
name|alloc_nr
argument_list|(
operator|*
name|offset
operator|+
name|pathlen
operator|+
literal|100
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
argument_list|,
operator|*
name|size
argument_list|)
expr_stmt|;
block|}
operator|*
name|offset
operator|+=
name|sprintf
argument_list|(
name|buffer
operator|+
operator|*
name|offset
argument_list|,
literal|"%.*s%c%d %d\n"
argument_list|,
name|pathlen
argument_list|,
name|path
argument_list|,
literal|0
argument_list|,
name|it
operator|->
name|entry_count
argument_list|,
name|it
operator|->
name|subtree_nr
argument_list|)
expr_stmt|;
if|#
directive|if
name|DEBUG
if|if
condition|(
literal|0
operator|<=
name|it
operator|->
name|entry_count
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree<%.*s> (%d ent, %d subtree) %s\n"
argument_list|,
name|pathlen
argument_list|,
name|path
argument_list|,
name|it
operator|->
name|entry_count
argument_list|,
name|it
operator|->
name|subtree_nr
argument_list|,
name|sha1_to_hex
argument_list|(
name|it
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree<%.*s> (%d subtree) invalid\n"
argument_list|,
name|pathlen
argument_list|,
name|path
argument_list|,
name|it
operator|->
name|subtree_nr
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
literal|0
operator|<=
name|it
operator|->
name|entry_count
condition|)
block|{
name|hashcpy
argument_list|(
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buffer
operator|+
operator|*
name|offset
argument_list|,
name|it
operator|->
name|sha1
argument_list|)
expr_stmt|;
operator|*
name|offset
operator|+=
literal|20
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|it
operator|->
name|subtree_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cache_tree_sub
modifier|*
name|down
init|=
name|it
operator|->
name|down
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|i
condition|)
block|{
name|struct
name|cache_tree_sub
modifier|*
name|prev
init|=
name|it
operator|->
name|down
index|[
name|i
operator|-
literal|1
index|]
decl_stmt|;
if|if
condition|(
name|subtree_name_cmp
argument_list|(
name|down
operator|->
name|name
argument_list|,
name|down
operator|->
name|namelen
argument_list|,
name|prev
operator|->
name|name
argument_list|,
name|prev
operator|->
name|namelen
argument_list|)
operator|<=
literal|0
condition|)
name|die
argument_list|(
literal|"fatal - unsorted cache subtree"
argument_list|)
expr_stmt|;
block|}
name|buffer
operator|=
name|write_one
argument_list|(
name|down
operator|->
name|cache_tree
argument_list|,
name|down
operator|->
name|name
argument_list|,
name|down
operator|->
name|namelen
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|,
name|offset
argument_list|)
expr_stmt|;
block|}
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_write
name|void
modifier|*
name|cache_tree_write
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|root
parameter_list|,
name|unsigned
name|long
modifier|*
name|size_p
parameter_list|)
block|{
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|unsigned
name|long
name|size
init|=
literal|8192
decl_stmt|;
name|char
modifier|*
name|buffer
init|=
name|xmalloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
operator|*
name|size_p
operator|=
literal|0
expr_stmt|;
name|path
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|write_one
argument_list|(
name|root
argument_list|,
name|path
argument_list|,
literal|0
argument_list|,
name|buffer
argument_list|,
operator|&
name|size
argument_list|,
name|size_p
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_one
specifier|static
name|struct
name|cache_tree
modifier|*
name|read_one
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
modifier|*
name|size_p
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|buf
init|=
operator|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
init|=
operator|*
name|size_p
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|char
modifier|*
name|ep
decl_stmt|;
name|struct
name|cache_tree
modifier|*
name|it
decl_stmt|;
name|int
name|i
decl_stmt|,
name|subtree_nr
decl_stmt|;
name|it
operator|=
name|NULL
expr_stmt|;
comment|/* skip name, but make sure name exists */
while|while
condition|(
name|size
operator|&&
operator|*
name|buf
condition|)
block|{
name|size
operator|--
expr_stmt|;
name|buf
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|size
condition|)
goto|goto
name|free_return
goto|;
name|buf
operator|++
expr_stmt|;
name|size
operator|--
expr_stmt|;
name|it
operator|=
name|cache_tree
argument_list|()
expr_stmt|;
name|cp
operator|=
name|buf
expr_stmt|;
name|it
operator|->
name|entry_count
operator|=
name|strtol
argument_list|(
name|cp
argument_list|,
operator|&
name|ep
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|cp
operator|==
name|ep
condition|)
goto|goto
name|free_return
goto|;
name|cp
operator|=
name|ep
expr_stmt|;
name|subtree_nr
operator|=
name|strtol
argument_list|(
name|cp
argument_list|,
operator|&
name|ep
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|cp
operator|==
name|ep
condition|)
goto|goto
name|free_return
goto|;
while|while
condition|(
name|size
operator|&&
operator|*
name|buf
operator|&&
operator|*
name|buf
operator|!=
literal|'\n'
condition|)
block|{
name|size
operator|--
expr_stmt|;
name|buf
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|size
condition|)
goto|goto
name|free_return
goto|;
name|buf
operator|++
expr_stmt|;
name|size
operator|--
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|it
operator|->
name|entry_count
condition|)
block|{
if|if
condition|(
name|size
operator|<
literal|20
condition|)
goto|goto
name|free_return
goto|;
name|hashcpy
argument_list|(
name|it
operator|->
name|sha1
argument_list|,
operator|(
name|unsigned
name|char
operator|*
operator|)
name|buf
argument_list|)
expr_stmt|;
name|buf
operator|+=
literal|20
expr_stmt|;
name|size
operator|-=
literal|20
expr_stmt|;
block|}
if|#
directive|if
name|DEBUG
if|if
condition|(
literal|0
operator|<=
name|it
operator|->
name|entry_count
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree<%s> (%d ent, %d subtree) %s\n"
argument_list|,
operator|*
name|buffer
argument_list|,
name|it
operator|->
name|entry_count
argument_list|,
name|subtree_nr
argument_list|,
name|sha1_to_hex
argument_list|(
name|it
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"cache-tree<%s> (%d subtrees) invalid\n"
argument_list|,
operator|*
name|buffer
argument_list|,
name|subtree_nr
argument_list|)
expr_stmt|;
endif|#
directive|endif
comment|/* 	 * Just a heuristic -- we do not add directories that often but 	 * we do not want to have to extend it immediately when we do, 	 * hence +2. 	 */
name|it
operator|->
name|subtree_alloc
operator|=
name|subtree_nr
operator|+
literal|2
expr_stmt|;
name|it
operator|->
name|down
operator|=
name|xcalloc
argument_list|(
name|it
operator|->
name|subtree_alloc
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|cache_tree_sub
operator|*
argument_list|)
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
name|subtree_nr
condition|;
name|i
operator|++
control|)
block|{
comment|/* read each subtree */
name|struct
name|cache_tree
modifier|*
name|sub
decl_stmt|;
name|struct
name|cache_tree_sub
modifier|*
name|subtree
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|buf
decl_stmt|;
name|sub
operator|=
name|read_one
argument_list|(
operator|&
name|buf
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sub
condition|)
goto|goto
name|free_return
goto|;
name|subtree
operator|=
name|cache_tree_sub
argument_list|(
name|it
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|subtree
operator|->
name|cache_tree
operator|=
name|sub
expr_stmt|;
block|}
if|if
condition|(
name|subtree_nr
operator|!=
name|it
operator|->
name|subtree_nr
condition|)
name|die
argument_list|(
literal|"cache-tree: internal error"
argument_list|)
expr_stmt|;
operator|*
name|buffer
operator|=
name|buf
expr_stmt|;
operator|*
name|size_p
operator|=
name|size
expr_stmt|;
return|return
name|it
return|;
name|free_return
label|:
name|cache_tree_free
argument_list|(
operator|&
name|it
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_read
name|struct
name|cache_tree
modifier|*
name|cache_tree_read
parameter_list|(
specifier|const
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
if|if
condition|(
name|buffer
index|[
literal|0
index|]
condition|)
return|return
name|NULL
return|;
comment|/* not the whole tree */
return|return
name|read_one
argument_list|(
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cache_tree_find
name|struct
name|cache_tree
modifier|*
name|cache_tree_find
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
while|while
condition|(
operator|*
name|path
condition|)
block|{
specifier|const
name|char
modifier|*
name|slash
decl_stmt|;
name|struct
name|cache_tree_sub
modifier|*
name|sub
decl_stmt|;
name|slash
operator|=
name|strchr
argument_list|(
name|path
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|slash
condition|)
name|slash
operator|=
name|path
operator|+
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
comment|/* between path and slash is the name of the 		 * subtree to look for. 		 */
name|sub
operator|=
name|find_subtree
argument_list|(
name|it
argument_list|,
name|path
argument_list|,
name|slash
operator|-
name|path
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sub
condition|)
return|return
name|NULL
return|;
name|it
operator|=
name|sub
operator|->
name|cache_tree
expr_stmt|;
if|if
condition|(
name|slash
condition|)
while|while
condition|(
operator|*
name|slash
operator|&&
operator|*
name|slash
operator|==
literal|'/'
condition|)
name|slash
operator|++
expr_stmt|;
if|if
condition|(
operator|!
name|slash
operator|||
operator|!
operator|*
name|slash
condition|)
return|return
name|it
return|;
comment|/* prefix ended with slashes */
name|path
operator|=
name|slash
expr_stmt|;
block|}
return|return
name|it
return|;
block|}
end_function
end_unit
