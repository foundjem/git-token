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
file|"diff.h"
end_include
begin_decl_stmt
DECL|variable|show_diff_usage
specifier|static
specifier|const
name|char
modifier|*
name|show_diff_usage
init|=
literal|"show-diff [-q] [-s] [-z] [-p] [paths...]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|generate_patch
specifier|static
name|int
name|generate_patch
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|line_termination
specifier|static
name|int
name|line_termination
init|=
literal|'\n'
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|silent
specifier|static
name|int
name|silent
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|silent_on_nonexisting_files
specifier|static
name|int
name|silent_on_nonexisting_files
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|matches_pathspec
specifier|static
name|int
name|matches_pathspec
parameter_list|(
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|,
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
name|ce_namelen
argument_list|(
name|ce
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
name|ce
operator|->
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
name|ce
operator|->
name|name
index|[
name|speclen
index|]
operator|==
literal|0
operator|||
name|ce
operator|->
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
DECL|function|show_unmerge
specifier|static
name|void
name|show_unmerge
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
if|if
condition|(
name|generate_patch
condition|)
name|diff_unmerge
argument_list|(
name|path
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"U %s%c"
argument_list|,
name|path
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_file
specifier|static
name|void
name|show_file
parameter_list|(
name|int
name|pfx
parameter_list|,
name|struct
name|cache_entry
modifier|*
name|ce
parameter_list|)
block|{
if|if
condition|(
name|generate_patch
condition|)
name|diff_addremove
argument_list|(
name|pfx
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
else|else
name|printf
argument_list|(
literal|"%c%06o\t%s\t%s\t%s%c"
argument_list|,
name|pfx
argument_list|,
name|ntohl
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
argument_list|,
literal|"blob"
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_modified
specifier|static
name|void
name|show_modified
parameter_list|(
name|int
name|oldmode
parameter_list|,
name|int
name|mode
parameter_list|,
specifier|const
name|char
modifier|*
name|old_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
name|old_sha1_hex
index|[
literal|41
index|]
decl_stmt|;
name|strcpy
argument_list|(
name|old_sha1_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|old_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|generate_patch
condition|)
name|diff_change
argument_list|(
name|oldmode
argument_list|,
name|mode
argument_list|,
name|old_sha1
argument_list|,
name|sha1
argument_list|,
name|path
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"*%06o->%06o\tblob\t%s->%s\t%s%c"
argument_list|,
name|oldmode
argument_list|,
name|mode
argument_list|,
name|old_sha1_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|path
argument_list|,
name|line_termination
argument_list|)
expr_stmt|;
block|}
end_function
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
specifier|static
specifier|const
name|char
name|null_sha1
index|[
literal|20
index|]
init|=
block|{
literal|0
block|, }
decl_stmt|;
name|int
name|entries
init|=
name|read_cache
argument_list|()
decl_stmt|;
name|int
name|i
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
operator|&&
name|argv
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
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-s"
argument_list|)
condition|)
name|silent_on_nonexisting_files
operator|=
name|silent
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-p"
argument_list|)
condition|)
name|generate_patch
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-q"
argument_list|)
condition|)
name|silent_on_nonexisting_files
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-z"
argument_list|)
condition|)
name|line_termination
operator|=
literal|0
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-r"
argument_list|)
condition|)
empty_stmt|;
comment|/* no-op */
else|else
name|usage
argument_list|(
name|show_diff_usage
argument_list|)
expr_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
comment|/* At this point, if argc == 1, then we are doing everything. 	 * Otherwise argv[1] .. argv[argc-1] have the explicit paths. 	 */
if|if
condition|(
name|entries
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"read_cache"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
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
name|mode
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
literal|1
operator|<
name|argc
operator|&&
operator|!
name|matches_pathspec
argument_list|(
name|ce
argument_list|,
name|argv
operator|+
literal|1
argument_list|,
name|argc
operator|-
literal|1
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
name|show_unmerge
argument_list|(
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|entries
operator|&&
operator|!
name|strcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|active_cache
index|[
name|i
index|]
operator|->
name|name
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
name|i
operator|--
expr_stmt|;
comment|/* compensate for loop control increments */
continue|continue;
block|}
if|if
condition|(
name|stat
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
name|silent_on_nonexisting_files
condition|)
continue|continue;
name|show_file
argument_list|(
literal|'-'
argument_list|,
name|ce
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|changed
operator|=
name|cache_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|changed
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
name|mode
operator|=
name|S_IFREG
operator||
name|ce_permissions
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
name|show_modified
argument_list|(
name|oldmode
argument_list|,
name|mode
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
name|null_sha1
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
