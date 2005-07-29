begin_unit
begin_comment
comment|/*  * Check-out files from the "current cache directory"  *  * Copyright (C) 2005 Linus Torvalds  *  * Careful: order of argument flags does matter. For example,  *  *	git-checkout-cache -a -f file.c  *  * Will first check out all files listed in the cache (but not  * overwrite any old ones), and then force-checkout "file.c" a  * second time (ie that one _will_ overwrite any old contents  * with the same filename).  *  * Also, just doing "git-checkout-cache" does nothing. You probably  * meant "git-checkout-cache -a". And if you want to force it, you  * want "git-checkout-cache -f -a".  *  * Intuitiveness is not the goal here. Repeatability is. The  * reason for the "no arguments means no work" thing is that  * from scripts you are supposed to be able to do things like  *  *	find . -name '*.h' -print0 | xargs -0 git-checkout-cache -f --  *  * which will force all existing *.h files to be replaced with  * their cached copies. If an empty command line implied "all",  * then this would force-refresh everything in the cache, which  * was not the point.  *  * Oh, and the "--" is just a good idea when you know the rest  * will be filenames. Just so that you wouldn't have a filename  * of "-a" causing problems (not possible in the above example,  * but get used to it in scripting!).  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|state
specifier|static
name|struct
name|checkout
name|state
init|=
block|{
operator|.
name|base_dir
operator|=
literal|""
block|,
operator|.
name|base_dir_len
operator|=
literal|0
block|,
operator|.
name|force
operator|=
literal|0
block|,
operator|.
name|quiet
operator|=
literal|0
block|,
operator|.
name|not_new
operator|=
literal|0
block|,
operator|.
name|refresh_cache
operator|=
literal|0
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|checkout_file
specifier|static
name|int
name|checkout_file
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|pos
init|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|strlen
argument_list|(
name|name
argument_list|)
argument_list|)
decl_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|state
operator|.
name|quiet
condition|)
block|{
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git-checkout-cache: %s is %s.\n"
argument_list|,
name|name
argument_list|,
operator|(
name|pos
operator|<
name|active_nr
operator|&&
operator|!
name|strcmp
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|name
argument_list|,
name|name
argument_list|)
operator|)
condition|?
literal|"unmerged"
else|:
literal|"not in the cache"
argument_list|)
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
return|return
name|checkout_entry
argument_list|(
name|active_cache
index|[
name|pos
index|]
argument_list|,
operator|&
name|state
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|checkout_all
specifier|static
name|int
name|checkout_all
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
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|checkout_entry
argument_list|(
name|ce
argument_list|,
operator|&
name|state
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|checkout_cache_usage
specifier|static
specifier|const
name|char
name|checkout_cache_usage
index|[]
init|=
literal|"git-checkout-cache [-u] [-q] [-a] [-f] [-n] [--prefix=<string>] [--]<file>..."
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
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|force_filename
init|=
literal|0
decl_stmt|;
name|struct
name|cache_file
name|cache_file
decl_stmt|;
name|int
name|newfd
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
block|{
name|die
argument_list|(
literal|"invalid cache"
argument_list|)
expr_stmt|;
block|}
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
name|force_filename
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-a"
argument_list|)
condition|)
block|{
name|checkout_all
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
literal|"--"
argument_list|)
condition|)
block|{
name|force_filename
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
literal|"-f"
argument_list|)
condition|)
block|{
name|state
operator|.
name|force
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
literal|"-q"
argument_list|)
condition|)
block|{
name|state
operator|.
name|quiet
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
literal|"-n"
argument_list|)
condition|)
block|{
name|state
operator|.
name|not_new
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
literal|"-u"
argument_list|)
condition|)
block|{
name|state
operator|.
name|refresh_cache
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|newfd
operator|<
literal|0
condition|)
name|newfd
operator|=
name|hold_index_file_for_update
argument_list|(
operator|&
name|cache_file
argument_list|,
name|get_index_file
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
name|newfd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"cannot open index.lock file."
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|arg
argument_list|,
literal|"--prefix="
argument_list|,
literal|9
argument_list|)
condition|)
block|{
name|state
operator|.
name|base_dir
operator|=
name|arg
operator|+
literal|9
expr_stmt|;
name|state
operator|.
name|base_dir_len
operator|=
name|strlen
argument_list|(
name|state
operator|.
name|base_dir
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|arg
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
name|usage
argument_list|(
name|checkout_cache_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|state
operator|.
name|base_dir_len
condition|)
block|{
comment|/* when --prefix is specified we do not 			 * want to update cache. 			 */
if|if
condition|(
name|state
operator|.
name|refresh_cache
condition|)
block|{
name|close
argument_list|(
name|newfd
argument_list|)
expr_stmt|;
name|newfd
operator|=
operator|-
literal|1
expr_stmt|;
name|rollback_index_file
argument_list|(
operator|&
name|cache_file
argument_list|)
expr_stmt|;
block|}
name|state
operator|.
name|refresh_cache
operator|=
literal|0
expr_stmt|;
block|}
name|checkout_file
argument_list|(
name|arg
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
literal|0
operator|<=
name|newfd
operator|&&
operator|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_index_file
argument_list|(
operator|&
name|cache_file
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"Unable to write new cachefile"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
