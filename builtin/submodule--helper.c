begin_unit
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
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"pathspec.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_include
include|#
directive|include
file|"submodule.h"
end_include
begin_include
include|#
directive|include
file|"submodule-config.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_struct
DECL|struct|module_list
struct|struct
name|module_list
block|{
DECL|member|entries
specifier|const
name|struct
name|cache_entry
modifier|*
modifier|*
name|entries
decl_stmt|;
DECL|member|alloc
DECL|member|nr
name|int
name|alloc
decl_stmt|,
name|nr
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|MODULE_LIST_INIT
define|#
directive|define
name|MODULE_LIST_INIT
value|{ NULL, 0, 0 }
end_define
begin_function
DECL|function|module_list_compute
specifier|static
name|int
name|module_list_compute
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
parameter_list|,
name|struct
name|pathspec
modifier|*
name|pathspec
parameter_list|,
name|struct
name|module_list
modifier|*
name|list
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|result
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|ps_matched
init|=
name|NULL
decl_stmt|;
name|parse_pathspec
argument_list|(
name|pathspec
argument_list|,
literal|0
argument_list|,
name|PATHSPEC_PREFER_FULL
operator||
name|PATHSPEC_STRIP_SUBMODULE_SLASH_CHEAP
argument_list|,
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathspec
operator|->
name|nr
condition|)
name|ps_matched
operator|=
name|xcalloc
argument_list|(
name|pathspec
operator|->
name|nr
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"index file corrupt"
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
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
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
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|||
operator|!
name|match_pathspec
argument_list|(
name|pathspec
argument_list|,
name|ce
operator|->
name|name
argument_list|,
name|ce_namelen
argument_list|(
name|ce
argument_list|)
argument_list|,
literal|0
argument_list|,
name|ps_matched
argument_list|,
literal|1
argument_list|)
condition|)
continue|continue;
name|ALLOC_GROW
argument_list|(
name|list
operator|->
name|entries
argument_list|,
name|list
operator|->
name|nr
operator|+
literal|1
argument_list|,
name|list
operator|->
name|alloc
argument_list|)
expr_stmt|;
name|list
operator|->
name|entries
index|[
name|list
operator|->
name|nr
operator|++
index|]
operator|=
name|ce
expr_stmt|;
while|while
condition|(
name|i
operator|+
literal|1
operator|<
name|active_nr
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
operator|+
literal|1
index|]
operator|->
name|name
argument_list|)
condition|)
comment|/* 			 * Skip entries with the same name in different stages 			 * to make sure an entry is returned only once. 			 */
name|i
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|ps_matched
operator|&&
name|report_path_error
argument_list|(
name|ps_matched
argument_list|,
name|pathspec
argument_list|,
name|prefix
argument_list|)
condition|)
name|result
operator|=
operator|-
literal|1
expr_stmt|;
name|free
argument_list|(
name|ps_matched
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|module_list
specifier|static
name|int
name|module_list
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
name|struct
name|pathspec
name|pathspec
decl_stmt|;
name|struct
name|module_list
name|list
init|=
name|MODULE_LIST_INIT
decl_stmt|;
name|struct
name|option
name|module_list_options
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"prefix"
argument_list|,
operator|&
name|prefix
argument_list|,
name|N_
argument_list|(
literal|"path"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"alternative anchor for relative paths"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
specifier|const
name|git_submodule_helper_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git submodule--helper list [--prefix=<path>] [<path>...]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|module_list_options
argument_list|,
name|git_submodule_helper_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|module_list_compute
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
operator|&
name|pathspec
argument_list|,
operator|&
name|list
argument_list|)
operator|<
literal|0
condition|)
block|{
name|printf
argument_list|(
literal|"#unmatched\n"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|list
operator|.
name|entries
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
name|printf
argument_list|(
literal|"%06o %s U\t"
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|,
name|sha1_to_hex
argument_list|(
name|null_sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%06o %s %d\t"
argument_list|,
name|ce
operator|->
name|ce_mode
argument_list|,
name|sha1_to_hex
argument_list|(
name|ce
operator|->
name|sha1
argument_list|)
argument_list|,
name|ce_stage
argument_list|(
name|ce
argument_list|)
argument_list|)
expr_stmt|;
name|utf8_fprintf
argument_list|(
name|stdout
argument_list|,
literal|"%s\n"
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
begin_function
DECL|function|module_name
specifier|static
name|int
name|module_name
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
specifier|const
name|struct
name|submodule
modifier|*
name|sub
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|2
condition|)
name|usage
argument_list|(
name|_
argument_list|(
literal|"git submodule--helper name<path>"
argument_list|)
argument_list|)
expr_stmt|;
name|gitmodules_config
argument_list|()
expr_stmt|;
name|sub
operator|=
name|submodule_from_path
argument_list|(
name|null_sha1
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|sub
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"no submodule mapping found in .gitmodules for path '%s'"
argument_list|)
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sub
operator|->
name|name
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|clone_submodule
specifier|static
name|int
name|clone_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|gitdir
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|,
specifier|const
name|char
modifier|*
name|depth
parameter_list|,
specifier|const
name|char
modifier|*
name|reference
parameter_list|,
name|int
name|quiet
parameter_list|)
block|{
name|struct
name|child_process
name|cp
decl_stmt|;
name|child_process_init
argument_list|(
operator|&
name|cp
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"clone"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"--no-checkout"
argument_list|)
expr_stmt|;
if|if
condition|(
name|quiet
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"--quiet"
argument_list|)
expr_stmt|;
if|if
condition|(
name|depth
operator|&&
operator|*
name|depth
condition|)
name|argv_array_pushl
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"--depth"
argument_list|,
name|depth
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|reference
operator|&&
operator|*
name|reference
condition|)
name|argv_array_pushl
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"--reference"
argument_list|,
name|reference
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|gitdir
operator|&&
operator|*
name|gitdir
condition|)
name|argv_array_pushl
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
literal|"--separate-git-dir"
argument_list|,
name|gitdir
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cp
operator|.
name|args
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|cp
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|cp
operator|.
name|env
operator|=
name|local_repo_env
expr_stmt|;
name|cp
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
return|return
name|run_command
argument_list|(
operator|&
name|cp
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|module_clone
specifier|static
name|int
name|module_clone
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
specifier|const
name|char
modifier|*
name|path
init|=
name|NULL
decl_stmt|,
modifier|*
name|name
init|=
name|NULL
decl_stmt|,
modifier|*
name|url
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|reference
init|=
name|NULL
decl_stmt|,
modifier|*
name|depth
init|=
name|NULL
decl_stmt|;
name|int
name|quiet
init|=
literal|0
decl_stmt|;
name|FILE
modifier|*
name|submodule_dot_git
decl_stmt|;
name|char
modifier|*
name|sm_gitdir
decl_stmt|,
modifier|*
name|cwd
decl_stmt|,
modifier|*
name|p
decl_stmt|;
name|struct
name|strbuf
name|rel_path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|option
name|module_clone_options
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"prefix"
argument_list|,
operator|&
name|prefix
argument_list|,
name|N_
argument_list|(
literal|"path"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"alternative anchor for relative paths"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"path"
argument_list|,
operator|&
name|path
argument_list|,
name|N_
argument_list|(
literal|"path"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"where the new submodule will be cloned to"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"name"
argument_list|,
operator|&
name|name
argument_list|,
name|N_
argument_list|(
literal|"string"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"name of the new submodule"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"url"
argument_list|,
operator|&
name|url
argument_list|,
name|N_
argument_list|(
literal|"string"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"url where to clone the submodule from"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"reference"
argument_list|,
operator|&
name|reference
argument_list|,
name|N_
argument_list|(
literal|"string"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"reference repository"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"depth"
argument_list|,
operator|&
name|depth
argument_list|,
name|N_
argument_list|(
literal|"string"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"depth for shallow clones"
argument_list|)
argument_list|)
block|,
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
literal|"Suppress output for cloning a submodule"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
specifier|const
name|git_submodule_helper_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git submodule--helper clone [--prefix=<path>] [--quiet] "
literal|"[--reference<repository>] [--name<name>] [--url<url>]"
literal|"[--depth<depth>] [--] [<path>...]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|module_clone_options
argument_list|,
name|git_submodule_helper_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/modules/%s"
argument_list|,
name|get_git_dir
argument_list|()
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|sm_gitdir
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|sb
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|file_exists
argument_list|(
name|sm_gitdir
argument_list|)
condition|)
block|{
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|sm_gitdir
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"could not create directory '%s'"
argument_list|)
argument_list|,
name|sm_gitdir
argument_list|)
expr_stmt|;
if|if
condition|(
name|clone_submodule
argument_list|(
name|path
argument_list|,
name|sm_gitdir
argument_list|,
name|url
argument_list|,
name|depth
argument_list|,
name|reference
argument_list|,
name|quiet
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"clone of '%s' into submodule path '%s' failed"
argument_list|)
argument_list|,
name|url
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|path
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"could not create directory '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/index"
argument_list|,
name|sm_gitdir
argument_list|)
expr_stmt|;
name|unlink_or_warn
argument_list|(
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
comment|/* Write a .git file in the submodule to redirect to the superproject. */
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|path
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"could not create directory '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|path
operator|&&
operator|*
name|path
condition|)
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/.git"
argument_list|,
name|path
argument_list|)
expr_stmt|;
else|else
name|strbuf_addstr
argument_list|(
operator|&
name|sb
argument_list|,
literal|".git"
argument_list|)
expr_stmt|;
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|sb
operator|.
name|buf
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"could not create leading directories of '%s'"
argument_list|)
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|submodule_dot_git
operator|=
name|fopen
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
literal|"w"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|submodule_dot_git
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"cannot open file '%s'"
argument_list|)
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|submodule_dot_git
argument_list|,
literal|"gitdir: %s\n"
argument_list|,
name|relative_path
argument_list|(
name|sm_gitdir
argument_list|,
name|path
argument_list|,
operator|&
name|rel_path
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|fclose
argument_list|(
name|submodule_dot_git
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"could not close file %s"
argument_list|)
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|rel_path
argument_list|)
expr_stmt|;
name|cwd
operator|=
name|xgetcwd
argument_list|()
expr_stmt|;
comment|/* Redirect the worktree of the submodule in the superproject's config */
if|if
condition|(
operator|!
name|is_absolute_path
argument_list|(
name|sm_gitdir
argument_list|)
condition|)
block|{
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/%s"
argument_list|,
name|cwd
argument_list|,
name|sm_gitdir
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|sm_gitdir
argument_list|)
expr_stmt|;
name|sm_gitdir
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|sb
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|"%s/%s"
argument_list|,
name|cwd
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|p
operator|=
name|git_pathdup_submodule
argument_list|(
name|path
argument_list|,
literal|"config"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"could not get submodule directory for '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|git_config_set_in_file
argument_list|(
name|p
argument_list|,
literal|"core.worktree"
argument_list|,
name|relative_path
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|sm_gitdir
argument_list|,
operator|&
name|rel_path
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|rel_path
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|sm_gitdir
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|cwd
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
end_function
begin_struct
DECL|struct|cmd_struct
struct|struct
name|cmd_struct
block|{
DECL|member|cmd
specifier|const
name|char
modifier|*
name|cmd
decl_stmt|;
DECL|member|fn
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
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|commands
specifier|static
name|struct
name|cmd_struct
name|commands
index|[]
init|=
block|{
block|{
literal|"list"
block|,
name|module_list
block|}
block|,
block|{
literal|"name"
block|,
name|module_name
block|}
block|,
block|{
literal|"clone"
block|,
name|module_clone
block|}
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_submodule__helper
name|int
name|cmd_submodule__helper
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
if|if
condition|(
name|argc
operator|<
literal|2
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"fatal: submodule--helper subcommand must be "
literal|"called with a subcommand"
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
name|ARRAY_SIZE
argument_list|(
name|commands
argument_list|)
condition|;
name|i
operator|++
control|)
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
name|commands
index|[
name|i
index|]
operator|.
name|cmd
argument_list|)
condition|)
return|return
name|commands
index|[
name|i
index|]
operator|.
name|fn
argument_list|(
name|argc
operator|-
literal|1
argument_list|,
name|argv
operator|+
literal|1
argument_list|,
name|prefix
argument_list|)
return|;
name|die
argument_list|(
name|_
argument_list|(
literal|"fatal: '%s' is not a valid submodule--helper "
literal|"subcommand"
argument_list|)
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
