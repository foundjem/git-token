begin_unit
begin_comment
comment|/*  * git gc builtin command  *  * Cleanup unreachable files and optimize the repository.  *  * Copyright (c) 2007 James Bowes  *  * Based on git-gc.sh, which is  *  * Copyright (c) 2006 Shawn O. Pearce  */
end_comment
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_define
DECL|macro|FAILED_RUN
define|#
directive|define
name|FAILED_RUN
value|"failed to run %s"
end_define
begin_decl_stmt
DECL|variable|builtin_gc_usage
specifier|static
specifier|const
name|char
name|builtin_gc_usage
index|[]
init|=
literal|"git-gc [--prune] [--aggressive]"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pack_refs
specifier|static
name|int
name|pack_refs
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|aggressive_window
specifier|static
name|int
name|aggressive_window
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|gc_auto_threshold
specifier|static
name|int
name|gc_auto_threshold
init|=
literal|6700
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|MAX_ADD
define|#
directive|define
name|MAX_ADD
value|10
end_define
begin_decl_stmt
DECL|variable|argv_pack_refs
specifier|static
specifier|const
name|char
modifier|*
name|argv_pack_refs
index|[]
init|=
block|{
literal|"pack-refs"
block|,
literal|"--all"
block|,
literal|"--prune"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|argv_reflog
specifier|static
specifier|const
name|char
modifier|*
name|argv_reflog
index|[]
init|=
block|{
literal|"reflog"
block|,
literal|"expire"
block|,
literal|"--all"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|argv_repack
specifier|static
specifier|const
name|char
modifier|*
name|argv_repack
index|[
name|MAX_ADD
index|]
init|=
block|{
literal|"repack"
block|,
literal|"-a"
block|,
literal|"-d"
block|,
literal|"-l"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|argv_prune
specifier|static
specifier|const
name|char
modifier|*
name|argv_prune
index|[]
init|=
block|{
literal|"prune"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|argv_rerere
specifier|static
specifier|const
name|char
modifier|*
name|argv_rerere
index|[]
init|=
block|{
literal|"rerere"
block|,
literal|"gc"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|argv_repack_auto
specifier|static
specifier|const
name|char
modifier|*
name|argv_repack_auto
index|[]
init|=
block|{
literal|"repack"
block|,
literal|"-d"
block|,
literal|"-l"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|gc_config
specifier|static
name|int
name|gc_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"gc.packrefs"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|value
argument_list|,
literal|"notbare"
argument_list|)
condition|)
name|pack_refs
operator|=
operator|-
literal|1
expr_stmt|;
else|else
name|pack_refs
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"gc.aggressivewindow"
argument_list|)
condition|)
block|{
name|aggressive_window
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"gc.auto"
argument_list|)
condition|)
block|{
name|gc_auto_threshold
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|append_option
specifier|static
name|void
name|append_option
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|cmd
parameter_list|,
specifier|const
name|char
modifier|*
name|opt
parameter_list|,
name|int
name|max_length
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
name|cmd
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
empty_stmt|;
if|if
condition|(
name|i
operator|+
literal|2
operator|>=
name|max_length
condition|)
name|die
argument_list|(
literal|"Too many options specified"
argument_list|)
expr_stmt|;
name|cmd
index|[
name|i
operator|++
index|]
operator|=
name|opt
expr_stmt|;
name|cmd
index|[
name|i
index|]
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|need_to_gc
specifier|static
name|int
name|need_to_gc
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* 	 * Quickly check if a "gc" is needed, by estimating how 	 * many loose objects there are.  Because SHA-1 is evenly 	 * distributed, we can check only one and get a reasonable 	 * estimate. 	 */
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|objdir
init|=
name|get_object_directory
argument_list|()
decl_stmt|;
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|ent
decl_stmt|;
name|int
name|auto_threshold
decl_stmt|;
name|int
name|num_loose
init|=
literal|0
decl_stmt|;
name|int
name|needed
init|=
literal|0
decl_stmt|;
comment|/* 	 * Setting gc.auto to 0 or negative can disable the 	 * automatic gc 	 */
if|if
condition|(
name|gc_auto_threshold
operator|<=
literal|0
condition|)
return|return
literal|0
return|;
if|if
condition|(
sizeof|sizeof
argument_list|(
name|path
argument_list|)
operator|<=
name|snprintf
argument_list|(
name|path
argument_list|,
sizeof|sizeof
argument_list|(
name|path
argument_list|)
argument_list|,
literal|"%s/17"
argument_list|,
name|objdir
argument_list|)
condition|)
block|{
name|warning
argument_list|(
literal|"insanely long object directory %.*s"
argument_list|,
literal|50
argument_list|,
name|objdir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|dir
operator|=
name|opendir
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return
literal|0
return|;
name|auto_threshold
operator|=
operator|(
name|gc_auto_threshold
operator|+
literal|255
operator|)
operator|/
literal|256
expr_stmt|;
while|while
condition|(
operator|(
name|ent
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
name|strspn
argument_list|(
name|ent
operator|->
name|d_name
argument_list|,
literal|"0123456789abcdef"
argument_list|)
operator|!=
literal|38
operator|||
name|ent
operator|->
name|d_name
index|[
literal|38
index|]
operator|!=
literal|'\0'
condition|)
continue|continue;
if|if
condition|(
operator|++
name|num_loose
operator|>
name|auto_threshold
condition|)
block|{
name|needed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
name|needed
return|;
block|}
end_function
begin_function
DECL|function|cmd_gc
name|int
name|cmd_gc
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
name|prefix
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|prune
init|=
literal|0
decl_stmt|;
name|int
name|auto_gc
init|=
literal|0
decl_stmt|;
name|char
name|buf
index|[
literal|80
index|]
decl_stmt|;
name|git_config
argument_list|(
name|gc_config
argument_list|)
expr_stmt|;
if|if
condition|(
name|pack_refs
operator|<
literal|0
condition|)
name|pack_refs
operator|=
operator|!
name|is_bare_repository
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
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--prune"
argument_list|)
condition|)
block|{
name|prune
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
name|append_option
argument_list|(
name|argv_repack
argument_list|,
literal|"-f"
argument_list|,
name|MAX_ADD
argument_list|)
expr_stmt|;
if|if
condition|(
name|aggressive_window
operator|>
literal|0
condition|)
block|{
name|sprintf
argument_list|(
name|buf
argument_list|,
literal|"--window=%d"
argument_list|,
name|aggressive_window
argument_list|)
expr_stmt|;
name|append_option
argument_list|(
name|argv_repack
argument_list|,
name|buf
argument_list|,
name|MAX_ADD
argument_list|)
expr_stmt|;
block|}
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--auto"
argument_list|)
condition|)
block|{
name|auto_gc
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
if|if
condition|(
name|i
operator|!=
name|argc
condition|)
name|usage
argument_list|(
name|builtin_gc_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|auto_gc
condition|)
block|{
comment|/* 		 * Auto-gc should be least intrusive as possible. 		 */
name|prune
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
name|ARRAY_SIZE
argument_list|(
name|argv_repack_auto
argument_list|)
condition|;
name|i
operator|++
control|)
name|argv_repack
index|[
name|i
index|]
operator|=
name|argv_repack_auto
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|need_to_gc
argument_list|()
condition|)
return|return
literal|0
return|;
block|}
if|if
condition|(
name|pack_refs
operator|&&
name|run_command_v_opt
argument_list|(
name|argv_pack_refs
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|argv_pack_refs
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|argv_reflog
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|argv_reflog
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|argv_repack
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|argv_repack
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|prune
operator|&&
name|run_command_v_opt
argument_list|(
name|argv_prune
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|argv_prune
index|[
literal|0
index|]
argument_list|)
return|;
if|if
condition|(
name|run_command_v_opt
argument_list|(
name|argv_rerere
argument_list|,
name|RUN_GIT_CMD
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|FAILED_RUN
argument_list|,
name|argv_rerere
index|[
literal|0
index|]
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
