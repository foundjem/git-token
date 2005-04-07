begin_unit
begin_comment
comment|/*  * GIT - The information manager from hell  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
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
name|char
modifier|*
name|sha1_dir
init|=
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
decl_stmt|,
modifier|*
name|path
decl_stmt|;
name|int
name|len
decl_stmt|,
name|i
decl_stmt|,
name|fd
decl_stmt|;
if|if
condition|(
name|mkdir
argument_list|(
literal|".dircache"
argument_list|,
literal|0700
argument_list|)
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"unable to create .dircache"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * If you want to, you can share the DB area with any number of branches. 	 * That has advantages: you can save space by sharing all the SHA1 objects. 	 * On the other hand, it might just make lookup slower and messier. You 	 * be the judge. 	 */
name|sha1_dir
operator|=
name|getenv
argument_list|(
name|DB_ENVIRONMENT
argument_list|)
expr_stmt|;
if|if
condition|(
name|sha1_dir
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|sha1_dir
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
operator|&&
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
return|return;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"DB_ENVIRONMENT set to bad directory %s: "
argument_list|,
name|sha1_dir
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * The default case is to have a DB per managed directory.  	 */
name|sha1_dir
operator|=
name|DEFAULT_DB_ENVIRONMENT
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"defaulting to private storage area\n"
argument_list|)
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|sha1_dir
argument_list|)
expr_stmt|;
if|if
condition|(
name|mkdir
argument_list|(
name|sha1_dir
argument_list|,
literal|0700
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|EEXIST
condition|)
block|{
name|perror
argument_list|(
name|sha1_dir
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
name|path
operator|=
name|malloc
argument_list|(
name|len
operator|+
literal|40
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|path
argument_list|,
name|sha1_dir
argument_list|,
name|len
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
literal|256
condition|;
name|i
operator|++
control|)
block|{
name|sprintf
argument_list|(
name|path
operator|+
name|len
argument_list|,
literal|"/%02x"
argument_list|,
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
name|mkdir
argument_list|(
name|path
argument_list|,
literal|0700
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|EEXIST
condition|)
block|{
name|perror
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
