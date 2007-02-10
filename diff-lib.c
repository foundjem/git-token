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
file|"quote.h"
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
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_comment
comment|/*  * diff-files  */
end_comment
begin_function
DECL|function|run_diff_files
name|int
name|run_diff_files
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|silent_on_removed
parameter_list|)
block|{
name|int
name|entries
decl_stmt|,
name|i
decl_stmt|;
name|int
name|diff_unmerged_stage
init|=
name|revs
operator|->
name|max_count
decl_stmt|;
if|if
condition|(
name|diff_unmerged_stage
operator|<
literal|0
condition|)
name|diff_unmerged_stage
operator|=
literal|2
expr_stmt|;
name|entries
operator|=
name|active_nr
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
name|stat
name|st
decl_stmt|;
name|unsigned
name|int
name|oldmode
decl_stmt|,
name|newmode
decl_stmt|;
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
name|int
name|changed
decl_stmt|;
if|if
condition|(
operator|!
name|ce_path_match
argument_list|(
name|ce
argument_list|,
name|revs
operator|->
name|prune_data
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
name|struct
name|combine_diff_path
modifier|*
name|dpath
decl_stmt|;
name|int
name|num_compare_stages
init|=
literal|0
decl_stmt|;
name|size_t
name|path_len
decl_stmt|;
name|path_len
operator|=
name|ce_namelen
argument_list|(
name|ce
argument_list|)
expr_stmt|;
name|dpath
operator|=
name|xmalloc
argument_list|(
name|combine_diff_path_size
argument_list|(
literal|5
argument_list|,
name|path_len
argument_list|)
argument_list|)
expr_stmt|;
name|dpath
operator|->
name|path
operator|=
operator|(
name|char
operator|*
operator|)
operator|&
operator|(
name|dpath
operator|->
name|parent
index|[
literal|5
index|]
operator|)
expr_stmt|;
name|dpath
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|dpath
operator|->
name|len
operator|=
name|path_len
expr_stmt|;
name|memcpy
argument_list|(
name|dpath
operator|->
name|path
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|path_len
argument_list|)
expr_stmt|;
name|dpath
operator|->
name|path
index|[
name|path_len
index|]
operator|=
literal|'\0'
expr_stmt|;
name|dpath
operator|->
name|mode
operator|=
literal|0
expr_stmt|;
name|hashclr
argument_list|(
name|dpath
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
operator|(
name|dpath
operator|->
name|parent
index|[
literal|0
index|]
operator|)
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|combine_diff_parent
argument_list|)
operator|*
literal|5
argument_list|)
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|entries
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|nce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
name|int
name|stage
decl_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|nce
operator|->
name|name
argument_list|)
condition|)
break|break;
comment|/* Stage #2 (ours) is the first parent, 				 * stage #3 (theirs) is the second. 				 */
name|stage
operator|=
name|ce_stage
argument_list|(
name|nce
argument_list|)
expr_stmt|;
if|if
condition|(
literal|2
operator|<=
name|stage
condition|)
block|{
name|int
name|mode
init|=
name|ntohl
argument_list|(
name|nce
operator|->
name|ce_mode
argument_list|)
decl_stmt|;
name|num_compare_stages
operator|++
expr_stmt|;
name|hashcpy
argument_list|(
name|dpath
operator|->
name|parent
index|[
name|stage
operator|-
literal|2
index|]
operator|.
name|sha1
argument_list|,
name|nce
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|dpath
operator|->
name|parent
index|[
name|stage
operator|-
literal|2
index|]
operator|.
name|mode
operator|=
name|canon_mode
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|dpath
operator|->
name|parent
index|[
name|stage
operator|-
literal|2
index|]
operator|.
name|status
operator|=
name|DIFF_STATUS_MODIFIED
expr_stmt|;
block|}
comment|/* diff against the proper unmerged stage */
if|if
condition|(
name|stage
operator|==
name|diff_unmerged_stage
condition|)
name|ce
operator|=
name|nce
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
comment|/* 			 * Compensate for loop update 			 */
name|i
operator|--
expr_stmt|;
if|if
condition|(
name|revs
operator|->
name|combine_merges
operator|&&
name|num_compare_stages
operator|==
literal|2
condition|)
block|{
name|show_combined_diff
argument_list|(
name|dpath
argument_list|,
literal|2
argument_list|,
name|revs
operator|->
name|dense_combined_merges
argument_list|,
name|revs
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|dpath
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|free
argument_list|(
name|dpath
argument_list|)
expr_stmt|;
name|dpath
operator|=
name|NULL
expr_stmt|;
comment|/* 			 * Show the diff for the 'ce' if we found the one 			 * from the desired stage. 			 */
name|diff_unmerge
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|ce
operator|->
name|name
argument_list|,
literal|0
argument_list|,
name|null_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
operator|!=
name|diff_unmerged_stage
condition|)
continue|continue;
block|}
if|if
condition|(
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
operator|&&
name|errno
operator|!=
name|ENOTDIR
condition|)
block|{
name|perror
argument_list|(
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|silent_on_removed
condition|)
continue|continue;
name|diff_addremove
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
literal|'-'
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|changed
operator|=
name|ce_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|changed
operator|&&
operator|!
name|revs
operator|->
name|diffopt
operator|.
name|find_copies_harder
condition|)
continue|continue;
name|oldmode
operator|=
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|newmode
operator|=
name|canon_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|trust_executable_bit
operator|&&
name|S_ISREG
argument_list|(
name|newmode
argument_list|)
operator|&&
name|S_ISREG
argument_list|(
name|oldmode
argument_list|)
operator|&&
operator|(
operator|(
name|newmode
operator|^
name|oldmode
operator|)
operator|==
literal|0111
operator|)
condition|)
name|newmode
operator|=
name|oldmode
expr_stmt|;
name|diff_change
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|oldmode
argument_list|,
name|newmode
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
operator|(
name|changed
condition|?
name|null_sha1
else|:
name|ce
operator|->
name|sha1
operator|)
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
name|diffcore_std
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * diff-index  */
end_comment
begin_comment
comment|/* A file entry went away or appeared */
end_comment
begin_function
DECL|function|diff_index_show_file
specifier|static
name|void
name|diff_index_show_file
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|)
block|{
name|diff_addremove
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|prefix
index|[
literal|0
index|]
argument_list|,
name|ntohl
argument_list|(
name|mode
argument_list|)
argument_list|,
name|sha1
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_stat_data
specifier|static
name|int
name|get_stat_data
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
name|unsigned
name|char
modifier|*
modifier|*
name|sha1p
parameter_list|,
name|unsigned
name|int
modifier|*
name|modep
parameter_list|,
name|int
name|cached
parameter_list|,
name|int
name|match_missing
parameter_list|)
block|{
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|ce
operator|->
name|sha1
decl_stmt|;
name|unsigned
name|int
name|mode
init|=
name|ce
operator|->
name|ce_mode
decl_stmt|;
if|if
condition|(
operator|!
name|cached
condition|)
block|{
specifier|static
name|unsigned
name|char
name|no_sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|changed
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|ENOENT
operator|&&
name|match_missing
condition|)
block|{
operator|*
name|sha1p
operator|=
name|sha1
expr_stmt|;
operator|*
name|modep
operator|=
name|mode
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
operator|-
literal|1
return|;
block|}
name|changed
operator|=
name|ce_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|changed
condition|)
block|{
name|mode
operator|=
name|create_ce_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|trust_executable_bit
operator|&&
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
name|mode
operator|=
name|ce
operator|->
name|ce_mode
expr_stmt|;
name|sha1
operator|=
name|no_sha1
expr_stmt|;
block|}
block|}
operator|*
name|sha1p
operator|=
name|sha1
expr_stmt|;
operator|*
name|modep
operator|=
name|mode
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|show_new_file
specifier|static
name|void
name|show_new_file
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|new
parameter_list|,
name|int
name|cached
parameter_list|,
name|int
name|match_missing
parameter_list|)
block|{
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
comment|/* New file in the index: it might actually be different in 	 * the working copy. 	 */
if|if
condition|(
name|get_stat_data
argument_list|(
name|new
argument_list|,
operator|&
name|sha1
argument_list|,
operator|&
name|mode
argument_list|,
name|cached
argument_list|,
name|match_missing
argument_list|)
operator|<
literal|0
condition|)
return|return;
name|diff_index_show_file
argument_list|(
name|revs
argument_list|,
literal|"+"
argument_list|,
name|new
argument_list|,
name|sha1
argument_list|,
name|mode
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_modified
specifier|static
name|int
name|show_modified
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|old
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|new
parameter_list|,
name|int
name|report_missing
parameter_list|,
name|int
name|cached
parameter_list|,
name|int
name|match_missing
parameter_list|)
block|{
name|unsigned
name|int
name|mode
decl_stmt|,
name|oldmode
decl_stmt|;
name|unsigned
name|char
modifier|*
name|sha1
decl_stmt|;
if|if
condition|(
name|get_stat_data
argument_list|(
name|new
argument_list|,
operator|&
name|sha1
argument_list|,
operator|&
name|mode
argument_list|,
name|cached
argument_list|,
name|match_missing
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|report_missing
condition|)
name|diff_index_show_file
argument_list|(
name|revs
argument_list|,
literal|"-"
argument_list|,
name|old
argument_list|,
name|old
operator|->
name|sha1
argument_list|,
name|old
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|revs
operator|->
name|combine_merges
operator|&&
operator|!
name|cached
operator|&&
operator|(
name|hashcmp
argument_list|(
name|sha1
argument_list|,
name|old
operator|->
name|sha1
argument_list|)
operator|||
name|hashcmp
argument_list|(
name|old
operator|->
name|sha1
argument_list|,
name|new
operator|->
name|sha1
argument_list|)
operator|)
condition|)
block|{
name|struct
name|combine_diff_path
modifier|*
name|p
decl_stmt|;
name|int
name|pathlen
init|=
name|ce_namelen
argument_list|(
name|new
argument_list|)
decl_stmt|;
name|p
operator|=
name|xmalloc
argument_list|(
name|combine_diff_path_size
argument_list|(
literal|2
argument_list|,
name|pathlen
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|->
name|path
operator|=
operator|(
name|char
operator|*
operator|)
operator|&
name|p
operator|->
name|parent
index|[
literal|2
index|]
expr_stmt|;
name|p
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|p
operator|->
name|len
operator|=
name|pathlen
expr_stmt|;
name|memcpy
argument_list|(
name|p
operator|->
name|path
argument_list|,
name|new
operator|->
name|name
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
name|p
operator|->
name|path
index|[
name|pathlen
index|]
operator|=
literal|0
expr_stmt|;
name|p
operator|->
name|mode
operator|=
name|ntohl
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|hashclr
argument_list|(
name|p
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|p
operator|->
name|parent
argument_list|,
literal|0
argument_list|,
literal|2
operator|*
sizeof|sizeof
argument_list|(
expr|struct
name|combine_diff_parent
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|->
name|parent
index|[
literal|0
index|]
operator|.
name|status
operator|=
name|DIFF_STATUS_MODIFIED
expr_stmt|;
name|p
operator|->
name|parent
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|ntohl
argument_list|(
name|new
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|p
operator|->
name|parent
index|[
literal|0
index|]
operator|.
name|sha1
argument_list|,
name|new
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|p
operator|->
name|parent
index|[
literal|1
index|]
operator|.
name|status
operator|=
name|DIFF_STATUS_MODIFIED
expr_stmt|;
name|p
operator|->
name|parent
index|[
literal|1
index|]
operator|.
name|mode
operator|=
name|ntohl
argument_list|(
name|old
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|p
operator|->
name|parent
index|[
literal|1
index|]
operator|.
name|sha1
argument_list|,
name|old
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|show_combined_diff
argument_list|(
name|p
argument_list|,
literal|2
argument_list|,
name|revs
operator|->
name|dense_combined_merges
argument_list|,
name|revs
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|p
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|oldmode
operator|=
name|old
operator|->
name|ce_mode
expr_stmt|;
if|if
condition|(
name|mode
operator|==
name|oldmode
operator|&&
operator|!
name|hashcmp
argument_list|(
name|sha1
argument_list|,
name|old
operator|->
name|sha1
argument_list|)
operator|&&
operator|!
name|revs
operator|->
name|diffopt
operator|.
name|find_copies_harder
condition|)
return|return
literal|0
return|;
name|mode
operator|=
name|ntohl
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|oldmode
operator|=
name|ntohl
argument_list|(
name|oldmode
argument_list|)
expr_stmt|;
name|diff_change
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|oldmode
argument_list|,
name|mode
argument_list|,
name|old
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
name|old
operator|->
name|name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|diff_cache
specifier|static
name|int
name|diff_cache
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
name|ac
parameter_list|,
name|int
name|entries
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|,
name|int
name|cached
parameter_list|,
name|int
name|match_missing
parameter_list|)
block|{
while|while
condition|(
name|entries
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
operator|*
name|ac
decl_stmt|;
name|int
name|same
init|=
operator|(
name|entries
operator|>
literal|1
operator|)
operator|&&
name|ce_same_name
argument_list|(
name|ce
argument_list|,
name|ac
index|[
literal|1
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ce_path_match
argument_list|(
name|ce
argument_list|,
name|pathspec
argument_list|)
condition|)
goto|goto
name|skip_entry
goto|;
switch|switch
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
case|case
literal|0
case|:
comment|/* No stage 1 entry? That means it's a new file */
if|if
condition|(
operator|!
name|same
condition|)
block|{
name|show_new_file
argument_list|(
name|revs
argument_list|,
name|ce
argument_list|,
name|cached
argument_list|,
name|match_missing
argument_list|)
expr_stmt|;
break|break;
block|}
comment|/* Show difference between old and new */
name|show_modified
argument_list|(
name|revs
argument_list|,
name|ac
index|[
literal|1
index|]
argument_list|,
name|ce
argument_list|,
literal|1
argument_list|,
name|cached
argument_list|,
name|match_missing
argument_list|)
expr_stmt|;
break|break;
case|case
literal|1
case|:
comment|/* No stage 3 (merge) entry? 			 * That means it's been deleted. 			 */
if|if
condition|(
operator|!
name|same
condition|)
block|{
name|diff_index_show_file
argument_list|(
name|revs
argument_list|,
literal|"-"
argument_list|,
name|ce
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
break|break;
block|}
comment|/* We come here with ce pointing at stage 1 			 * (original tree) and ac[1] pointing at stage 			 * 3 (unmerged).  show-modified with 			 * report-missing set to false does not say the 			 * file is deleted but reports true if work 			 * tree does not have it, in which case we 			 * fall through to report the unmerged state. 			 * Otherwise, we show the differences between 			 * the original tree and the work tree. 			 */
if|if
condition|(
operator|!
name|cached
operator|&&
operator|!
name|show_modified
argument_list|(
name|revs
argument_list|,
name|ce
argument_list|,
name|ac
index|[
literal|1
index|]
argument_list|,
literal|0
argument_list|,
name|cached
argument_list|,
name|match_missing
argument_list|)
condition|)
break|break;
name|diff_unmerge
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|,
name|ce
operator|->
name|sha1
argument_list|)
expr_stmt|;
break|break;
case|case
literal|3
case|:
name|diff_unmerge
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|ce
operator|->
name|name
argument_list|,
literal|0
argument_list|,
name|null_sha1
argument_list|)
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"impossible cache entry stage"
argument_list|)
expr_stmt|;
block|}
name|skip_entry
label|:
comment|/* 		 * Ignore all the different stages for this file, 		 * we've handled the relevant cases now. 		 */
do|do
block|{
name|ac
operator|++
expr_stmt|;
name|entries
operator|--
expr_stmt|;
block|}
do|while
condition|(
name|entries
operator|&&
name|ce_same_name
argument_list|(
name|ce
argument_list|,
name|ac
index|[
literal|0
index|]
argument_list|)
condition|)
do|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * This turns all merge entries into "stage 3". That guarantees that  * when we read in the new tree (into "stage 1"), we won't lose sight  * of the fact that we had unmerged entries.  */
end_comment
begin_function
DECL|function|mark_merge_entries
specifier|static
name|void
name|mark_merge_entries
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
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
operator|!
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
name|ce
operator|->
name|ce_flags
operator||=
name|htons
argument_list|(
name|CE_STAGEMASK
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|run_diff_index
name|int
name|run_diff_index
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|cached
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
name|struct
name|object
modifier|*
name|ent
decl_stmt|;
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
specifier|const
name|char
modifier|*
name|tree_name
decl_stmt|;
name|int
name|match_missing
init|=
literal|0
decl_stmt|;
comment|/*  	 * Backward compatibility wart - "diff-index -m" does 	 * not mean "do not ignore merges", but totally different. 	 */
if|if
condition|(
operator|!
name|revs
operator|->
name|ignore_merges
condition|)
name|match_missing
operator|=
literal|1
expr_stmt|;
name|mark_merge_entries
argument_list|()
expr_stmt|;
name|ent
operator|=
name|revs
operator|->
name|pending
operator|.
name|objects
index|[
literal|0
index|]
operator|.
name|item
expr_stmt|;
name|tree_name
operator|=
name|revs
operator|->
name|pending
operator|.
name|objects
index|[
literal|0
index|]
operator|.
name|name
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|ent
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
return|return
name|error
argument_list|(
literal|"bad tree object %s"
argument_list|,
name|tree_name
argument_list|)
return|;
if|if
condition|(
name|read_tree
argument_list|(
name|tree
argument_list|,
literal|1
argument_list|,
name|revs
operator|->
name|prune_data
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"unable to read tree object %s"
argument_list|,
name|tree_name
argument_list|)
return|;
name|ret
operator|=
name|diff_cache
argument_list|(
name|revs
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|,
name|revs
operator|->
name|prune_data
argument_list|,
name|cached
argument_list|,
name|match_missing
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|do_diff_cache
name|int
name|do_diff_cache
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|tree_sha1
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|)
block|{
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
name|struct
name|rev_info
name|revs
decl_stmt|;
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
comment|/* 	 * This is used by git-blame to run diff-cache internally; 	 * it potentially needs to repeatedly run this, so we will 	 * start by removing the higher order entries the last round 	 * left behind. 	 */
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
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|revs
operator|.
name|prune_data
operator|=
name|opt
operator|->
name|paths
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|tree_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
name|die
argument_list|(
literal|"bad tree object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|tree_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_tree
argument_list|(
name|tree
argument_list|,
literal|1
argument_list|,
name|opt
operator|->
name|paths
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"unable to read tree %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|tree_sha1
argument_list|)
argument_list|)
return|;
return|return
name|diff_cache
argument_list|(
operator|&
name|revs
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|,
name|revs
operator|.
name|prune_data
argument_list|,
literal|1
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
end_unit
