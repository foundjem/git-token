begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_comment
comment|/*  * Returns the length (on a path component basis) of the longest  * common prefix match of 'name_a' and 'name_b'.  */
end_comment
begin_function
DECL|function|longest_path_match
specifier|static
name|int
name|longest_path_match
parameter_list|(
specifier|const
name|char
modifier|*
name|name_a
parameter_list|,
name|int
name|len_a
parameter_list|,
specifier|const
name|char
modifier|*
name|name_b
parameter_list|,
name|int
name|len_b
parameter_list|,
name|int
modifier|*
name|previous_slash
parameter_list|)
block|{
name|int
name|max_len
decl_stmt|,
name|match_len
init|=
literal|0
decl_stmt|,
name|match_len_prev
init|=
literal|0
decl_stmt|,
name|i
init|=
literal|0
decl_stmt|;
name|max_len
operator|=
name|len_a
operator|<
name|len_b
condition|?
name|len_a
else|:
name|len_b
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|max_len
operator|&&
name|name_a
index|[
name|i
index|]
operator|==
name|name_b
index|[
name|i
index|]
condition|)
block|{
if|if
condition|(
name|name_a
index|[
name|i
index|]
operator|==
literal|'/'
condition|)
block|{
name|match_len_prev
operator|=
name|match_len
expr_stmt|;
name|match_len
operator|=
name|i
expr_stmt|;
block|}
name|i
operator|++
expr_stmt|;
block|}
comment|/* 	 * Is 'name_b' a substring of 'name_a', the other way around, 	 * or is 'name_a' and 'name_b' the exact same string? 	 */
if|if
condition|(
name|i
operator|>=
name|max_len
operator|&&
operator|(
operator|(
name|len_a
operator|>
name|len_b
operator|&&
name|name_a
index|[
name|len_b
index|]
operator|==
literal|'/'
operator|)
operator|||
operator|(
name|len_a
operator|<
name|len_b
operator|&&
name|name_b
index|[
name|len_a
index|]
operator|==
literal|'/'
operator|)
operator|||
operator|(
name|len_a
operator|==
name|len_b
operator|)
operator|)
condition|)
block|{
name|match_len_prev
operator|=
name|match_len
expr_stmt|;
name|match_len
operator|=
name|i
expr_stmt|;
block|}
operator|*
name|previous_slash
operator|=
name|match_len_prev
expr_stmt|;
return|return
name|match_len
return|;
block|}
end_function
begin_struct
DECL|struct|cache_def
specifier|static
struct|struct
name|cache_def
block|{
DECL|member|path
name|char
name|path
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
DECL|member|len
name|int
name|len
decl_stmt|;
DECL|member|flags
name|int
name|flags
decl_stmt|;
DECL|member|track_flags
name|int
name|track_flags
decl_stmt|;
DECL|member|prefix_len_stat_func
name|int
name|prefix_len_stat_func
decl_stmt|;
block|}
DECL|variable|cache
name|cache
struct|;
end_struct
begin_function
DECL|function|reset_lstat_cache
specifier|static
specifier|inline
name|void
name|reset_lstat_cache
parameter_list|(
name|void
parameter_list|)
block|{
name|cache
operator|.
name|path
index|[
literal|0
index|]
operator|=
literal|'\0'
expr_stmt|;
name|cache
operator|.
name|len
operator|=
literal|0
expr_stmt|;
name|cache
operator|.
name|flags
operator|=
literal|0
expr_stmt|;
comment|/* 	 * The track_flags and prefix_len_stat_func members is only 	 * set by the safeguard rule inside lstat_cache() 	 */
block|}
end_function
begin_define
DECL|macro|FL_DIR
define|#
directive|define
name|FL_DIR
value|(1<< 0)
end_define
begin_define
DECL|macro|FL_NOENT
define|#
directive|define
name|FL_NOENT
value|(1<< 1)
end_define
begin_define
DECL|macro|FL_SYMLINK
define|#
directive|define
name|FL_SYMLINK
value|(1<< 2)
end_define
begin_define
DECL|macro|FL_LSTATERR
define|#
directive|define
name|FL_LSTATERR
value|(1<< 3)
end_define
begin_define
DECL|macro|FL_ERR
define|#
directive|define
name|FL_ERR
value|(1<< 4)
end_define
begin_define
DECL|macro|FL_FULLPATH
define|#
directive|define
name|FL_FULLPATH
value|(1<< 5)
end_define
begin_comment
comment|/*  * Check if name 'name' of length 'len' has a symlink leading  * component, or if the directory exists and is real, or not.  *  * To speed up the check, some information is allowed to be cached.  * This can be indicated by the 'track_flags' argument, which also can  * be used to indicate that we should check the full path.  *  * The 'prefix_len_stat_func' parameter can be used to set the length  * of the prefix, where the cache should use the stat() function  * instead of the lstat() function to test each path component.  */
end_comment
begin_function
DECL|function|lstat_cache
specifier|static
name|int
name|lstat_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|int
name|track_flags
parameter_list|,
name|int
name|prefix_len_stat_func
parameter_list|)
block|{
name|int
name|match_len
decl_stmt|,
name|last_slash
decl_stmt|,
name|last_slash_dir
decl_stmt|,
name|previous_slash
decl_stmt|;
name|int
name|match_flags
decl_stmt|,
name|ret_flags
decl_stmt|,
name|save_flags
decl_stmt|,
name|max_len
decl_stmt|,
name|ret
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|cache
operator|.
name|track_flags
operator|!=
name|track_flags
operator|||
name|cache
operator|.
name|prefix_len_stat_func
operator|!=
name|prefix_len_stat_func
condition|)
block|{
comment|/* 		 * As a safeguard rule we clear the cache if the 		 * values of track_flags and/or prefix_len_stat_func 		 * does not match with the last supplied values. 		 */
name|reset_lstat_cache
argument_list|()
expr_stmt|;
name|cache
operator|.
name|track_flags
operator|=
name|track_flags
expr_stmt|;
name|cache
operator|.
name|prefix_len_stat_func
operator|=
name|prefix_len_stat_func
expr_stmt|;
name|match_len
operator|=
name|last_slash
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
comment|/* 		 * Check to see if we have a match from the cache for 		 * the 2 "excluding" path types. 		 */
name|match_len
operator|=
name|last_slash
operator|=
name|longest_path_match
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|cache
operator|.
name|path
argument_list|,
name|cache
operator|.
name|len
argument_list|,
operator|&
name|previous_slash
argument_list|)
expr_stmt|;
name|match_flags
operator|=
name|cache
operator|.
name|flags
operator|&
name|track_flags
operator|&
operator|(
name|FL_NOENT
operator||
name|FL_SYMLINK
operator|)
expr_stmt|;
if|if
condition|(
name|match_flags
operator|&&
name|match_len
operator|==
name|cache
operator|.
name|len
condition|)
return|return
name|match_flags
return|;
comment|/* 		 * If we now have match_len> 0, we would know that 		 * the matched part will always be a directory. 		 * 		 * Also, if we are tracking directories and 'name' is 		 * a substring of the cache on a path component basis, 		 * we can return immediately. 		 */
name|match_flags
operator|=
name|track_flags
operator|&
name|FL_DIR
expr_stmt|;
if|if
condition|(
name|match_flags
operator|&&
name|len
operator|==
name|match_len
condition|)
return|return
name|match_flags
return|;
block|}
comment|/* 	 * Okay, no match from the cache so far, so now we have to 	 * check the rest of the path components. 	 */
name|ret_flags
operator|=
name|FL_DIR
expr_stmt|;
name|last_slash_dir
operator|=
name|last_slash
expr_stmt|;
name|max_len
operator|=
name|len
operator|<
name|PATH_MAX
condition|?
name|len
else|:
name|PATH_MAX
expr_stmt|;
while|while
condition|(
name|match_len
operator|<
name|max_len
condition|)
block|{
do|do
block|{
name|cache
operator|.
name|path
index|[
name|match_len
index|]
operator|=
name|name
index|[
name|match_len
index|]
expr_stmt|;
name|match_len
operator|++
expr_stmt|;
block|}
do|while
condition|(
name|match_len
operator|<
name|max_len
operator|&&
name|name
index|[
name|match_len
index|]
operator|!=
literal|'/'
condition|)
do|;
if|if
condition|(
name|match_len
operator|>=
name|max_len
operator|&&
operator|!
operator|(
name|track_flags
operator|&
name|FL_FULLPATH
operator|)
condition|)
break|break;
name|last_slash
operator|=
name|match_len
expr_stmt|;
name|cache
operator|.
name|path
index|[
name|last_slash
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|last_slash
operator|<=
name|prefix_len_stat_func
condition|)
name|ret
operator|=
name|stat
argument_list|(
name|cache
operator|.
name|path
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
else|else
name|ret
operator|=
name|lstat
argument_list|(
name|cache
operator|.
name|path
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
block|{
name|ret_flags
operator|=
name|FL_LSTATERR
expr_stmt|;
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
name|ret_flags
operator||=
name|FL_NOENT
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|last_slash_dir
operator|=
name|last_slash
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|ret_flags
operator|=
name|FL_SYMLINK
expr_stmt|;
block|}
else|else
block|{
name|ret_flags
operator|=
name|FL_ERR
expr_stmt|;
block|}
break|break;
block|}
comment|/* 	 * At the end update the cache.  Note that max 3 different 	 * path types, FL_NOENT, FL_SYMLINK and FL_DIR, can be cached 	 * for the moment! 	 */
name|save_flags
operator|=
name|ret_flags
operator|&
name|track_flags
operator|&
operator|(
name|FL_NOENT
operator||
name|FL_SYMLINK
operator|)
expr_stmt|;
if|if
condition|(
name|save_flags
operator|&&
name|last_slash
operator|>
literal|0
operator|&&
name|last_slash
operator|<=
name|PATH_MAX
condition|)
block|{
name|cache
operator|.
name|path
index|[
name|last_slash
index|]
operator|=
literal|'\0'
expr_stmt|;
name|cache
operator|.
name|len
operator|=
name|last_slash
expr_stmt|;
name|cache
operator|.
name|flags
operator|=
name|save_flags
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|(
name|track_flags
operator|&
name|FL_DIR
operator|)
operator|&&
name|last_slash_dir
operator|>
literal|0
operator|&&
name|last_slash_dir
operator|<=
name|PATH_MAX
condition|)
block|{
comment|/* 		 * We have a separate test for the directory case, 		 * since it could be that we have found a symlink or a 		 * non-existing directory and the track_flags says 		 * that we cannot cache this fact, so the cache would 		 * then have been left empty in this case. 		 * 		 * But if we are allowed to track real directories, we 		 * can still cache the path components before the last 		 * one (the found symlink or non-existing component). 		 */
name|cache
operator|.
name|path
index|[
name|last_slash_dir
index|]
operator|=
literal|'\0'
expr_stmt|;
name|cache
operator|.
name|len
operator|=
name|last_slash_dir
expr_stmt|;
name|cache
operator|.
name|flags
operator|=
name|FL_DIR
expr_stmt|;
block|}
else|else
block|{
name|reset_lstat_cache
argument_list|()
expr_stmt|;
block|}
return|return
name|ret_flags
return|;
block|}
end_function
begin_comment
comment|/*  * Invalidate the given 'name' from the cache, if 'name' matches  * completely with the cache.  */
end_comment
begin_function
DECL|function|invalidate_lstat_cache
name|void
name|invalidate_lstat_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|int
name|match_len
decl_stmt|,
name|previous_slash
decl_stmt|;
name|match_len
operator|=
name|longest_path_match
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|cache
operator|.
name|path
argument_list|,
name|cache
operator|.
name|len
argument_list|,
operator|&
name|previous_slash
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|==
name|match_len
condition|)
block|{
if|if
condition|(
operator|(
name|cache
operator|.
name|track_flags
operator|&
name|FL_DIR
operator|)
operator|&&
name|previous_slash
operator|>
literal|0
condition|)
block|{
name|cache
operator|.
name|path
index|[
name|previous_slash
index|]
operator|=
literal|'\0'
expr_stmt|;
name|cache
operator|.
name|len
operator|=
name|previous_slash
expr_stmt|;
name|cache
operator|.
name|flags
operator|=
name|FL_DIR
expr_stmt|;
block|}
else|else
block|{
name|reset_lstat_cache
argument_list|()
expr_stmt|;
block|}
block|}
block|}
end_function
begin_comment
comment|/*  * Completely clear the contents of the cache  */
end_comment
begin_function
DECL|function|clear_lstat_cache
name|void
name|clear_lstat_cache
parameter_list|(
name|void
parameter_list|)
block|{
name|reset_lstat_cache
argument_list|()
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|USE_ONLY_LSTAT
define|#
directive|define
name|USE_ONLY_LSTAT
value|0
end_define
begin_comment
comment|/*  * Return non-zero if path 'name' has a leading symlink component  */
end_comment
begin_function
DECL|function|has_symlink_leading_path
name|int
name|has_symlink_leading_path
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
return|return
name|lstat_cache
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|FL_SYMLINK
operator||
name|FL_DIR
argument_list|,
name|USE_ONLY_LSTAT
argument_list|)
operator|&
name|FL_SYMLINK
return|;
block|}
end_function
begin_comment
comment|/*  * Return non-zero if path 'name' has a leading symlink component or  * if some leading path component does not exists.  */
end_comment
begin_function
DECL|function|has_symlink_or_noent_leading_path
name|int
name|has_symlink_or_noent_leading_path
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
return|return
name|lstat_cache
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|FL_SYMLINK
operator||
name|FL_NOENT
operator||
name|FL_DIR
argument_list|,
name|USE_ONLY_LSTAT
argument_list|)
operator|&
operator|(
name|FL_SYMLINK
operator||
name|FL_NOENT
operator|)
return|;
block|}
end_function
begin_comment
comment|/*  * Return non-zero if all path components of 'name' exists as a  * directory.  If prefix_len> 0, we will test with the stat()  * function instead of the lstat() function for a prefix length of  * 'prefix_len', thus we then allow for symlinks in the prefix part as  * long as those points to real existing directories.  */
end_comment
begin_function
DECL|function|has_dirs_only_path
name|int
name|has_dirs_only_path
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|,
name|int
name|prefix_len
parameter_list|)
block|{
return|return
name|lstat_cache
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|FL_DIR
operator||
name|FL_FULLPATH
argument_list|,
name|prefix_len
argument_list|)
operator|&
name|FL_DIR
return|;
block|}
end_function
begin_struct
DECL|struct|removal_def
specifier|static
struct|struct
name|removal_def
block|{
DECL|member|path
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
DECL|member|len
name|int
name|len
decl_stmt|;
block|}
DECL|variable|removal
name|removal
struct|;
end_struct
begin_function
DECL|function|do_remove_scheduled_dirs
specifier|static
name|void
name|do_remove_scheduled_dirs
parameter_list|(
name|int
name|new_len
parameter_list|)
block|{
while|while
condition|(
name|removal
operator|.
name|len
operator|>
name|new_len
condition|)
block|{
name|removal
operator|.
name|path
index|[
name|removal
operator|.
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
name|rmdir
argument_list|(
name|removal
operator|.
name|path
argument_list|)
condition|)
break|break;
do|do
block|{
name|removal
operator|.
name|len
operator|--
expr_stmt|;
block|}
do|while
condition|(
name|removal
operator|.
name|len
operator|>
name|new_len
operator|&&
name|removal
operator|.
name|path
index|[
name|removal
operator|.
name|len
index|]
operator|!=
literal|'/'
condition|)
do|;
block|}
name|removal
operator|.
name|len
operator|=
name|new_len
expr_stmt|;
block|}
end_function
begin_function
DECL|function|schedule_dir_for_removal
name|void
name|schedule_dir_for_removal
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|int
name|match_len
decl_stmt|,
name|last_slash
decl_stmt|,
name|i
decl_stmt|,
name|previous_slash
decl_stmt|;
name|match_len
operator|=
name|last_slash
operator|=
name|i
operator|=
name|longest_path_match
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|removal
operator|.
name|path
argument_list|,
name|removal
operator|.
name|len
argument_list|,
operator|&
name|previous_slash
argument_list|)
expr_stmt|;
comment|/* Find last slash inside 'name' */
while|while
condition|(
name|i
operator|<
name|len
condition|)
block|{
if|if
condition|(
name|name
index|[
name|i
index|]
operator|==
literal|'/'
condition|)
name|last_slash
operator|=
name|i
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
comment|/* 	 * If we are about to go down the directory tree, we check if 	 * we must first go upwards the tree, such that we then can 	 * remove possible empty directories as we go upwards. 	 */
if|if
condition|(
name|match_len
operator|<
name|last_slash
operator|&&
name|match_len
operator|<
name|removal
operator|.
name|len
condition|)
name|do_remove_scheduled_dirs
argument_list|(
name|match_len
argument_list|)
expr_stmt|;
comment|/* 	 * If we go deeper down the directory tree, we only need to 	 * save the new path components as we go down. 	 */
if|if
condition|(
name|match_len
operator|<
name|last_slash
condition|)
block|{
name|memcpy
argument_list|(
operator|&
name|removal
operator|.
name|path
index|[
name|match_len
index|]
argument_list|,
operator|&
name|name
index|[
name|match_len
index|]
argument_list|,
name|last_slash
operator|-
name|match_len
argument_list|)
expr_stmt|;
name|removal
operator|.
name|len
operator|=
name|last_slash
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|remove_scheduled_dirs
name|void
name|remove_scheduled_dirs
parameter_list|(
name|void
parameter_list|)
block|{
name|do_remove_scheduled_dirs
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
