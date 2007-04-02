begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_include
include|#
directive|include
file|"unpack-trees.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_decl_stmt
DECL|variable|trees
specifier|static
name|struct
name|object_list
modifier|*
name|trees
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|list_tree
specifier|static
name|int
name|list_tree
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|tree
modifier|*
name|tree
init|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
return|return
operator|-
literal|1
return|;
name|object_list_append
argument_list|(
operator|&
name|tree
operator|->
name|object
argument_list|,
operator|&
name|trees
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_cache_unmerged
specifier|static
name|int
name|read_cache_unmerged
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|cache_entry
modifier|*
modifier|*
name|dst
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|last
init|=
name|NULL
decl_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
name|dst
operator|=
name|active_cache
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|active_nr
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
name|active_cache
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
name|last
operator|&&
operator|!
name|strcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|last
operator|->
name|name
argument_list|)
condition|)
continue|continue;
name|cache_tree_invalidate_path
argument_list|(
name|active_cache_tree
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|last
operator|=
name|ce
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
literal|0
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|&=
operator|~
name|htons
argument_list|(
name|CE_STAGEMASK
argument_list|)
expr_stmt|;
block|}
operator|*
name|dst
operator|++
operator|=
name|ce
expr_stmt|;
block|}
name|active_nr
operator|=
name|dst
operator|-
name|active_cache
expr_stmt|;
return|return
operator|!
operator|!
name|last
return|;
block|}
end_function
begin_function
DECL|function|prime_cache_tree_rec
specifier|static
name|void
name|prime_cache_tree_rec
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|it
parameter_list|,
name|struct
name|tree
modifier|*
name|tree
parameter_list|)
block|{
name|struct
name|tree_desc
name|desc
decl_stmt|;
name|struct
name|name_entry
name|entry
decl_stmt|;
name|int
name|cnt
decl_stmt|;
name|hashcpy
argument_list|(
name|it
operator|->
name|sha1
argument_list|,
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|init_tree_desc
argument_list|(
operator|&
name|desc
argument_list|,
name|tree
operator|->
name|buffer
argument_list|,
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
name|cnt
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|tree_entry
argument_list|(
operator|&
name|desc
argument_list|,
operator|&
name|entry
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
name|cnt
operator|++
expr_stmt|;
else|else
block|{
name|struct
name|cache_tree_sub
modifier|*
name|sub
decl_stmt|;
name|struct
name|tree
modifier|*
name|subtree
init|=
name|lookup_tree
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|subtree
operator|->
name|object
operator|.
name|parsed
condition|)
name|parse_tree
argument_list|(
name|subtree
argument_list|)
expr_stmt|;
name|sub
operator|=
name|cache_tree_sub
argument_list|(
name|it
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
name|sub
operator|->
name|cache_tree
operator|=
name|cache_tree
argument_list|()
expr_stmt|;
name|prime_cache_tree_rec
argument_list|(
name|sub
operator|->
name|cache_tree
argument_list|,
name|subtree
argument_list|)
expr_stmt|;
name|cnt
operator|+=
name|sub
operator|->
name|cache_tree
operator|->
name|entry_count
expr_stmt|;
block|}
block|}
name|it
operator|->
name|entry_count
operator|=
name|cnt
expr_stmt|;
block|}
end_function
begin_function
DECL|function|prime_cache_tree
specifier|static
name|void
name|prime_cache_tree
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|tree
modifier|*
name|tree
init|=
operator|(
expr|struct
name|tree
operator|*
operator|)
name|trees
operator|->
name|item
decl_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
return|return;
name|active_cache_tree
operator|=
name|cache_tree
argument_list|()
expr_stmt|;
name|prime_cache_tree_rec
argument_list|(
name|active_cache_tree
argument_list|,
name|tree
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|read_tree_usage
specifier|static
specifier|const
name|char
name|read_tree_usage
index|[]
init|=
literal|"git-read-tree (<sha> | [[-m [--aggressive] | --reset | --prefix=<prefix>] [-u | -i]] [--exclude-per-directory=<gitignore>]<sha1> [<sha2> [<sha3>]])"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|lock_file
specifier|static
name|struct
name|lock_file
name|lock_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_read_tree
name|int
name|cmd_read_tree
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
name|unused_prefix
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|newfd
decl_stmt|,
name|stage
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|unpack_trees_options
name|opts
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
name|opts
operator|.
name|head_idx
operator|=
operator|-
literal|1
expr_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
name|newfd
operator|=
name|hold_lock_file_for_update
argument_list|(
operator|&
name|lock_file
argument_list|,
name|get_index_file
argument_list|()
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
comment|/* "-u" means "update", meaning that a merge will update 		 * the working tree. 		 */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-u"
argument_list|)
condition|)
block|{
name|opts
operator|.
name|update
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-v"
argument_list|)
condition|)
block|{
name|opts
operator|.
name|verbose_update
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
comment|/* "-i" means "index only", meaning that a merge will 		 * not even look at the working tree. 		 */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-i"
argument_list|)
condition|)
block|{
name|opts
operator|.
name|index_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
comment|/* "--prefix=<subdirectory>/" means keep the current index 		 *  entries and put the entries from the tree under the 		 * given subdirectory. 		 */
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--prefix="
argument_list|)
condition|)
block|{
if|if
condition|(
name|stage
operator|||
name|opts
operator|.
name|merge
operator|||
name|opts
operator|.
name|prefix
condition|)
name|usage
argument_list|(
name|read_tree_usage
argument_list|)
expr_stmt|;
name|opts
operator|.
name|prefix
operator|=
name|arg
operator|+
literal|9
expr_stmt|;
name|opts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
name|stage
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|read_cache_unmerged
argument_list|()
condition|)
name|die
argument_list|(
literal|"you need to resolve your current index first"
argument_list|)
expr_stmt|;
continue|continue;
block|}
comment|/* This differs from "-m" in that we'll silently ignore 		 * unmerged entries and overwrite working tree files that 		 * correspond to them. 		 */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--reset"
argument_list|)
condition|)
block|{
if|if
condition|(
name|stage
operator|||
name|opts
operator|.
name|merge
operator|||
name|opts
operator|.
name|prefix
condition|)
name|usage
argument_list|(
name|read_tree_usage
argument_list|)
expr_stmt|;
name|opts
operator|.
name|reset
operator|=
literal|1
expr_stmt|;
name|opts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
name|stage
operator|=
literal|1
expr_stmt|;
name|read_cache_unmerged
argument_list|()
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--trivial"
argument_list|)
condition|)
block|{
name|opts
operator|.
name|trivial_merges_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--aggressive"
argument_list|)
condition|)
block|{
name|opts
operator|.
name|aggressive
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
comment|/* "-m" stands for "merge", meaning we start in stage 1 */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-m"
argument_list|)
condition|)
block|{
if|if
condition|(
name|stage
operator|||
name|opts
operator|.
name|merge
operator|||
name|opts
operator|.
name|prefix
condition|)
name|usage
argument_list|(
name|read_tree_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_cache_unmerged
argument_list|()
condition|)
name|die
argument_list|(
literal|"you need to resolve your current index first"
argument_list|)
expr_stmt|;
name|stage
operator|=
literal|1
expr_stmt|;
name|opts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|arg
argument_list|,
literal|"--exclude-per-directory="
argument_list|)
condition|)
block|{
name|struct
name|dir_struct
modifier|*
name|dir
decl_stmt|;
if|if
condition|(
name|opts
operator|.
name|dir
condition|)
name|die
argument_list|(
literal|"more than one --exclude-per-directory are given."
argument_list|)
expr_stmt|;
name|dir
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|opts
operator|.
name|dir
argument_list|)
argument_list|)
expr_stmt|;
name|dir
operator|->
name|show_ignored
operator|=
literal|1
expr_stmt|;
name|dir
operator|->
name|exclude_per_dir
operator|=
name|arg
operator|+
literal|24
expr_stmt|;
name|opts
operator|.
name|dir
operator|=
name|dir
expr_stmt|;
comment|/* We do not need to nor want to do read-directory 			 * here; we are merely interested in reusing the 			 * per directory ignore stack mechanism. 			 */
continue|continue;
block|}
comment|/* using -u and -i at the same time makes no sense */
if|if
condition|(
literal|1
operator|<
name|opts
operator|.
name|index_only
operator|+
name|opts
operator|.
name|update
condition|)
name|usage
argument_list|(
name|read_tree_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Not a valid object name %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
name|list_tree
argument_list|(
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"failed to unpack tree object %s"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|stage
operator|++
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|opts
operator|.
name|update
operator|||
name|opts
operator|.
name|index_only
operator|)
operator|&&
operator|!
name|opts
operator|.
name|merge
condition|)
name|usage
argument_list|(
name|read_tree_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|opts
operator|.
name|dir
operator|&&
operator|!
name|opts
operator|.
name|update
operator|)
condition|)
name|die
argument_list|(
literal|"--exclude-per-directory is meaningless unless -u"
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|.
name|prefix
condition|)
block|{
name|int
name|pfxlen
init|=
name|strlen
argument_list|(
name|opts
operator|.
name|prefix
argument_list|)
decl_stmt|;
name|int
name|pos
decl_stmt|;
if|if
condition|(
name|opts
operator|.
name|prefix
index|[
name|pfxlen
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
name|die
argument_list|(
literal|"prefix must end with /"
argument_list|)
expr_stmt|;
if|if
condition|(
name|stage
operator|!=
literal|2
condition|)
name|die
argument_list|(
literal|"binding merge takes only one tree"
argument_list|)
expr_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|opts
operator|.
name|prefix
argument_list|,
name|pfxlen
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
name|die
argument_list|(
literal|"corrupt index file"
argument_list|)
expr_stmt|;
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|pos
operator|<
name|active_nr
operator|&&
operator|!
name|strncmp
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
argument_list|,
name|opts
operator|.
name|prefix
argument_list|,
name|pfxlen
argument_list|)
condition|)
name|die
argument_list|(
literal|"subdirectory '%s' already exists."
argument_list|,
name|opts
operator|.
name|prefix
argument_list|)
expr_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|opts
operator|.
name|prefix
argument_list|,
name|pfxlen
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
name|die
argument_list|(
literal|"file '%.*s' already exists."
argument_list|,
name|pfxlen
operator|-
literal|1
argument_list|,
name|opts
operator|.
name|prefix
argument_list|)
expr_stmt|;
name|opts
operator|.
name|pos
operator|=
operator|-
literal|1
operator|-
name|pos
expr_stmt|;
block|}
if|if
condition|(
name|opts
operator|.
name|merge
condition|)
block|{
if|if
condition|(
name|stage
operator|<
literal|2
condition|)
name|die
argument_list|(
literal|"just how do you expect me to merge %d trees?"
argument_list|,
name|stage
operator|-
literal|1
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|stage
operator|-
literal|1
condition|)
block|{
case|case
literal|1
case|:
name|opts
operator|.
name|fn
operator|=
name|opts
operator|.
name|prefix
condition|?
name|bind_merge
else|:
name|oneway_merge
expr_stmt|;
break|break;
case|case
literal|2
case|:
name|opts
operator|.
name|fn
operator|=
name|twoway_merge
expr_stmt|;
break|break;
case|case
literal|3
case|:
default|default:
name|opts
operator|.
name|fn
operator|=
name|threeway_merge
expr_stmt|;
name|cache_tree_free
argument_list|(
operator|&
name|active_cache_tree
argument_list|)
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|stage
operator|-
literal|1
operator|>=
literal|3
condition|)
name|opts
operator|.
name|head_idx
operator|=
name|stage
operator|-
literal|2
expr_stmt|;
else|else
name|opts
operator|.
name|head_idx
operator|=
literal|1
expr_stmt|;
block|}
name|unpack_trees
argument_list|(
name|trees
argument_list|,
operator|&
name|opts
argument_list|)
expr_stmt|;
comment|/* 	 * When reading only one tree (either the most basic form, 	 * "-m ent" or "--reset ent" form), we can obtain a fully 	 * valid cache-tree because the index must match exactly 	 * what came from the tree. 	 */
if|if
condition|(
name|trees
operator|&&
name|trees
operator|->
name|item
operator|&&
operator|!
name|opts
operator|.
name|prefix
operator|&&
operator|(
operator|!
name|opts
operator|.
name|merge
operator|||
operator|(
name|stage
operator|==
literal|2
operator|)
operator|)
condition|)
block|{
name|cache_tree_free
argument_list|(
operator|&
name|active_cache_tree
argument_list|)
expr_stmt|;
name|prime_cache_tree
argument_list|()
expr_stmt|;
block|}
if|if
condition|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|close
argument_list|(
name|newfd
argument_list|)
operator|||
name|commit_lock_file
argument_list|(
operator|&
name|lock_file
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to write new index file"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
