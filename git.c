begin_unit
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<sys/stat.h>
end_include
begin_include
include|#
directive|include
file|<dirent.h>
end_include
begin_include
include|#
directive|include
file|<unistd.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<errno.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|<stdarg.h>
end_include
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_function
DECL|function|prepend_to_path
specifier|static
name|void
name|prepend_to_path
parameter_list|(
specifier|const
name|char
modifier|*
name|dir
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|char
modifier|*
name|path
decl_stmt|,
modifier|*
name|old_path
init|=
name|getenv
argument_list|(
literal|"PATH"
argument_list|)
decl_stmt|;
name|int
name|path_len
init|=
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|old_path
condition|)
name|old_path
operator|=
literal|"/usr/local/bin:/usr/bin:/bin"
expr_stmt|;
name|path_len
operator|=
name|len
operator|+
name|strlen
argument_list|(
name|old_path
argument_list|)
operator|+
literal|1
expr_stmt|;
name|path
operator|=
name|malloc
argument_list|(
name|path_len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|path
argument_list|,
name|dir
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|path
index|[
name|len
index|]
operator|=
literal|':'
expr_stmt|;
name|memcpy
argument_list|(
name|path
operator|+
name|len
operator|+
literal|1
argument_list|,
name|old_path
argument_list|,
name|path_len
operator|-
name|len
argument_list|)
expr_stmt|;
name|setenv
argument_list|(
literal|"PATH"
argument_list|,
name|path
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|git_version_string
specifier|const
name|char
name|git_version_string
index|[]
init|=
name|GIT_VERSION
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|handle_internal_command
specifier|static
name|void
name|handle_internal_command
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
name|char
modifier|*
modifier|*
name|envp
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|cmd
init|=
name|argv
index|[
literal|0
index|]
decl_stmt|;
specifier|static
struct|struct
name|cmd_struct
block|{
specifier|const
name|char
modifier|*
name|cmd
decl_stmt|;
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
name|int
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
parameter_list|,
name|char
modifier|*
modifier|*
parameter_list|)
function_decl|;
block|}
name|commands
index|[]
init|=
block|{
block|{
literal|"version"
block|,
name|cmd_version
block|}
block|,
block|{
literal|"help"
block|,
name|cmd_help
block|}
block|,
block|{
literal|"log"
block|,
name|cmd_log
block|}
block|,
block|{
literal|"whatchanged"
block|,
name|cmd_whatchanged
block|}
block|,
block|{
literal|"show"
block|,
name|cmd_show
block|}
block|,
block|{
literal|"push"
block|,
name|cmd_push
block|}
block|,
block|{
literal|"count-objects"
block|,
name|cmd_count_objects
block|}
block|,
block|{
literal|"diff"
block|,
name|cmd_diff
block|}
block|,
block|{
literal|"grep"
block|,
name|cmd_grep
block|}
block|,
block|{
literal|"init-db"
block|,
name|cmd_init_db
block|}
block|,
block|{
literal|"tar-tree"
block|,
name|cmd_tar_tree
block|}
block|,
block|{
literal|"upload-tar"
block|,
name|cmd_upload_tar
block|}
block|,
block|{
literal|"rev-list"
block|,
name|cmd_rev_list
block|}
block|,
block|{
literal|"check-ref-format"
block|,
name|cmd_check_ref_format
block|}
block|}
struct|;
name|int
name|i
decl_stmt|;
comment|/* Turn "git cmd --help" into "git help cmd" */
if|if
condition|(
name|argc
operator|>
literal|1
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--help"
argument_list|)
condition|)
block|{
name|argv
index|[
literal|1
index|]
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|cmd
operator|=
literal|"help"
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
name|ARRAY_SIZE
argument_list|(
name|commands
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|cmd_struct
modifier|*
name|p
init|=
name|commands
operator|+
name|i
decl_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|p
operator|->
name|cmd
argument_list|,
name|cmd
argument_list|)
condition|)
continue|continue;
name|exit
argument_list|(
name|p
operator|->
name|fn
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|envp
argument_list|)
argument_list|)
expr_stmt|;
block|}
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
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|char
modifier|*
modifier|*
name|envp
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|cmd
init|=
name|argv
index|[
literal|0
index|]
decl_stmt|;
name|char
modifier|*
name|slash
init|=
name|strrchr
argument_list|(
name|cmd
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
name|char
name|git_command
index|[
name|PATH_MAX
operator|+
literal|1
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|exec_path
init|=
name|NULL
decl_stmt|;
comment|/* 	 * Take the basename of argv[0] as the command 	 * name, and the dirname as the default exec_path 	 * if it's an absolute path and we don't have 	 * anything better. 	 */
if|if
condition|(
name|slash
condition|)
block|{
operator|*
name|slash
operator|++
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|*
name|cmd
operator|==
literal|'/'
condition|)
name|exec_path
operator|=
name|cmd
expr_stmt|;
name|cmd
operator|=
name|slash
expr_stmt|;
block|}
comment|/* 	 * "git-xxxx" is the same as "git xxxx", but we obviously: 	 * 	 *  - cannot take flags in between the "git" and the "xxxx". 	 *  - cannot execute it externally (since it would just do 	 *    the same thing over again) 	 * 	 * So we just directly call the internal command handler, and 	 * die if that one cannot handle it. 	 */
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|cmd
argument_list|,
literal|"git-"
argument_list|,
literal|4
argument_list|)
condition|)
block|{
name|cmd
operator|+=
literal|4
expr_stmt|;
name|argv
index|[
literal|0
index|]
operator|=
name|cmd
expr_stmt|;
name|handle_internal_command
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|envp
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"cannot handle %s internally"
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
block|}
comment|/* Default command: "help" */
name|cmd
operator|=
literal|"help"
expr_stmt|;
comment|/* Look for flags.. */
while|while
condition|(
name|argc
operator|>
literal|1
condition|)
block|{
name|cmd
operator|=
operator|*
operator|++
name|argv
expr_stmt|;
name|argc
operator|--
expr_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
name|cmd
argument_list|,
literal|"--"
argument_list|,
literal|2
argument_list|)
condition|)
break|break;
name|cmd
operator|+=
literal|2
expr_stmt|;
comment|/* 		 * For legacy reasons, the "version" and "help" 		 * commands can be written with "--" prepended 		 * to make them look like flags. 		 */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|cmd
argument_list|,
literal|"help"
argument_list|)
condition|)
break|break;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|cmd
argument_list|,
literal|"version"
argument_list|)
condition|)
break|break;
comment|/* 		 * Check remaining flags (which by now must be 		 * "--exec-path", but maybe we will accept 		 * other arguments some day) 		 */
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|cmd
argument_list|,
literal|"exec-path"
argument_list|,
literal|9
argument_list|)
condition|)
block|{
name|cmd
operator|+=
literal|9
expr_stmt|;
if|if
condition|(
operator|*
name|cmd
operator|==
literal|'='
condition|)
block|{
name|git_set_exec_path
argument_list|(
name|cmd
operator|+
literal|1
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|puts
argument_list|(
name|git_exec_path
argument_list|()
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
name|cmd_usage
argument_list|(
literal|0
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
name|argv
index|[
literal|0
index|]
operator|=
name|cmd
expr_stmt|;
comment|/* 	 * We search for git commands in the following order: 	 *  - git_exec_path() 	 *  - the path of the "git" command if we could find it 	 *    in $0 	 *  - the regular PATH. 	 */
if|if
condition|(
name|exec_path
condition|)
name|prepend_to_path
argument_list|(
name|exec_path
argument_list|,
name|strlen
argument_list|(
name|exec_path
argument_list|)
argument_list|)
expr_stmt|;
name|exec_path
operator|=
name|git_exec_path
argument_list|()
expr_stmt|;
name|prepend_to_path
argument_list|(
name|exec_path
argument_list|,
name|strlen
argument_list|(
name|exec_path
argument_list|)
argument_list|)
expr_stmt|;
comment|/* See if it's an internal command */
name|handle_internal_command
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|envp
argument_list|)
expr_stmt|;
comment|/* .. then try the external ones */
name|execv_git_cmd
argument_list|(
name|argv
argument_list|)
expr_stmt|;
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
name|cmd_usage
argument_list|(
literal|0
argument_list|,
name|exec_path
argument_list|,
literal|"'%s' is not a git-command"
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Failed to run command '%s': %s\n"
argument_list|,
name|git_command
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
end_unit
