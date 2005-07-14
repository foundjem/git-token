begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_decl_stmt
DECL|variable|cached_only
specifier|static
name|int
name|cached_only
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_output_format
specifier|static
name|int
name|diff_output_format
init|=
name|DIFF_FORMAT_HUMAN
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|match_nonexisting
specifier|static
name|int
name|match_nonexisting
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|detect_rename
specifier|static
name|int
name|detect_rename
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|find_copies_harder
specifier|static
name|int
name|find_copies_harder
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_setup_opt
specifier|static
name|int
name|diff_setup_opt
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_score_opt
specifier|static
name|int
name|diff_score_opt
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pickaxe
specifier|static
specifier|const
name|char
modifier|*
name|pickaxe
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pickaxe_opts
specifier|static
name|int
name|pickaxe_opts
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_break_opt
specifier|static
name|int
name|diff_break_opt
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|orderfile
specifier|static
specifier|const
name|char
modifier|*
name|orderfile
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_filter
specifier|static
specifier|const
name|char
modifier|*
name|diff_filter
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* A file entry went away or appeared */
end_comment
begin_function
DECL|function|show_file
specifier|static
name|void
name|show_file
parameter_list|(
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
name|cached_only
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
name|match_nonexisting
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
name|cache_entry
modifier|*
name|new
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
comment|/* New file in the index: it might actually be different in the working copy */
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
argument_list|)
operator|<
literal|0
condition|)
return|return;
name|show_file
argument_list|(
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
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|report_missing
condition|)
name|show_file
argument_list|(
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
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|old
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
operator|&&
operator|!
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
name|ce
argument_list|)
expr_stmt|;
break|break;
block|}
comment|/* Show difference between old and new */
name|show_modified
argument_list|(
name|ac
index|[
literal|1
index|]
argument_list|,
name|ce
argument_list|,
literal|1
argument_list|)
expr_stmt|;
break|break;
case|case
literal|1
case|:
comment|/* No stage 3 (merge) entry? That means it's been deleted */
if|if
condition|(
operator|!
name|same
condition|)
block|{
name|show_file
argument_list|(
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
comment|/* We come here with ce pointing at stage 1 			 * (original tree) and ac[1] pointing at stage 			 * 3 (unmerged).  show-modified with 			 * report-mising set to false does not say the 			 * file is deleted but reports true if work 			 * tree does not have it, in which case we 			 * fall through to report the unmerged state. 			 * Otherwise, we show the differences between 			 * the original tree and the work tree. 			 */
if|if
condition|(
operator|!
name|cached_only
operator|&&
operator|!
name|show_modified
argument_list|(
name|ce
argument_list|,
name|ac
index|[
literal|1
index|]
argument_list|,
literal|0
argument_list|)
condition|)
break|break;
comment|/* fallthru */
case|case
literal|3
case|:
name|diff_unmerge
argument_list|(
name|ce
operator|->
name|name
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
begin_decl_stmt
DECL|variable|diff_cache_usage
specifier|static
name|char
modifier|*
name|diff_cache_usage
init|=
literal|"git-diff-cache [-m] [--cached] "
literal|"[<common diff options>]<tree-ish> [<path>...]"
name|COMMON_DIFF_OPTIONS_HELP
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|tree_name
init|=
name|NULL
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
init|=
name|NULL
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|int
name|allow_options
init|=
literal|1
decl_stmt|;
name|int
name|i
decl_stmt|;
name|read_cache
argument_list|()
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
if|if
condition|(
operator|!
name|allow_options
operator|||
operator|*
name|arg
operator|!=
literal|'-'
condition|)
block|{
if|if
condition|(
name|tree_name
condition|)
block|{
name|pathspec
operator|=
name|argv
operator|+
name|i
expr_stmt|;
break|break;
block|}
name|tree_name
operator|=
name|arg
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
literal|"--"
argument_list|)
condition|)
block|{
name|allow_options
operator|=
literal|0
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
literal|"-r"
argument_list|)
condition|)
block|{
comment|/* We accept the -r flag just to look like git-diff-tree */
continue|continue;
block|}
comment|/* We accept the -u flag as a synonym for "-p" */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-p"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-u"
argument_list|)
condition|)
block|{
name|diff_output_format
operator|=
name|DIFF_FORMAT_PATCH
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"-B"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
if|if
condition|(
operator|(
name|diff_break_opt
operator|=
name|diff_scoreopt_parse
argument_list|(
name|arg
argument_list|)
operator|)
operator|==
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|diff_cache_usage
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"-M"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|detect_rename
operator|=
name|DIFF_DETECT_RENAME
expr_stmt|;
if|if
condition|(
operator|(
name|diff_score_opt
operator|=
name|diff_scoreopt_parse
argument_list|(
name|arg
argument_list|)
operator|)
operator|==
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|diff_cache_usage
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"-C"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|detect_rename
operator|=
name|DIFF_DETECT_COPY
expr_stmt|;
if|if
condition|(
operator|(
name|diff_score_opt
operator|=
name|diff_scoreopt_parse
argument_list|(
name|arg
argument_list|)
operator|)
operator|==
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|diff_cache_usage
argument_list|)
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
literal|"--find-copies-harder"
argument_list|)
condition|)
block|{
name|find_copies_harder
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
literal|"-z"
argument_list|)
condition|)
block|{
name|diff_output_format
operator|=
name|DIFF_FORMAT_MACHINE
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
literal|"--name-only"
argument_list|)
condition|)
block|{
name|diff_output_format
operator|=
name|DIFF_FORMAT_NAME
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
literal|"--name-only-z"
argument_list|)
condition|)
block|{
name|diff_output_format
operator|=
name|DIFF_FORMAT_NAME_Z
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
literal|"-R"
argument_list|)
condition|)
block|{
name|diff_setup_opt
operator||=
name|DIFF_SETUP_REVERSE
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"-S"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|pickaxe
operator|=
name|arg
operator|+
literal|2
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--diff-filter="
argument_list|,
literal|14
argument_list|)
condition|)
block|{
name|diff_filter
operator|=
name|arg
operator|+
literal|14
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"-O"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
name|orderfile
operator|=
name|arg
operator|+
literal|2
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
literal|"--pickaxe-all"
argument_list|)
condition|)
block|{
name|pickaxe_opts
operator|=
name|DIFF_PICKAXE_ALL
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
literal|"-m"
argument_list|)
condition|)
block|{
name|match_nonexisting
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
literal|"--cached"
argument_list|)
condition|)
block|{
name|cached_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|diff_cache_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|find_copies_harder
operator|&&
name|detect_rename
operator|!=
name|DIFF_DETECT_COPY
condition|)
name|usage
argument_list|(
name|diff_cache_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree_name
operator|||
name|get_sha1
argument_list|(
name|tree_name
argument_list|,
name|sha1
argument_list|)
condition|)
name|usage
argument_list|(
name|diff_cache_usage
argument_list|)
expr_stmt|;
comment|/* The rest is for paths restriction. */
name|diff_setup
argument_list|(
name|diff_setup_opt
argument_list|)
expr_stmt|;
name|mark_merge_entries
argument_list|()
expr_stmt|;
name|tree
operator|=
name|read_object_with_reference
argument_list|(
name|sha1
argument_list|,
literal|"tree"
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
name|tree
condition|)
name|die
argument_list|(
literal|"bad tree object %s"
argument_list|,
name|tree_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_tree
argument_list|(
name|tree
argument_list|,
name|size
argument_list|,
literal|1
argument_list|,
name|pathspec
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to read tree object %s"
argument_list|,
name|tree_name
argument_list|)
expr_stmt|;
name|ret
operator|=
name|diff_cache
argument_list|(
name|active_cache
argument_list|,
name|active_nr
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
name|pathspec
argument_list|,
name|detect_rename
argument_list|,
name|diff_score_opt
argument_list|,
name|pickaxe
argument_list|,
name|pickaxe_opts
argument_list|,
name|diff_break_opt
argument_list|,
name|orderfile
argument_list|,
name|diff_filter
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
name|diff_output_format
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
