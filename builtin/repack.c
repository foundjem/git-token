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
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_decl_stmt
DECL|variable|delta_base_offset
specifier|static
name|int
name|delta_base_offset
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|packdir
DECL|variable|packtmp
specifier|static
name|char
modifier|*
name|packdir
decl_stmt|,
modifier|*
name|packtmp
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_repack_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|git_repack_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git repack [options]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|repack_config
specifier|static
name|int
name|repack_config
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
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"repack.usedeltabaseoffset"
argument_list|)
condition|)
block|{
name|delta_base_offset
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
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|cb
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * Remove temporary $GIT_OBJECT_DIRECTORY/pack/.tmp-$$-pack-* files.  */
end_comment
begin_function
DECL|function|remove_temporary_files
specifier|static
name|void
name|remove_temporary_files
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|size_t
name|dirlen
decl_stmt|,
name|prefixlen
decl_stmt|;
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|e
decl_stmt|;
name|dir
operator|=
name|opendir
argument_list|(
name|packdir
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return;
comment|/* Point at the slash at the end of ".../objects/pack/" */
name|dirlen
operator|=
name|strlen
argument_list|(
name|packdir
argument_list|)
operator|+
literal|1
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|packtmp
argument_list|)
expr_stmt|;
comment|/* Hold the length of  ".tmp-%d-pack-" */
name|prefixlen
operator|=
name|buf
operator|.
name|len
operator|-
name|dirlen
expr_stmt|;
while|while
condition|(
operator|(
name|e
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
condition|)
block|{
if|if
condition|(
name|strncmp
argument_list|(
name|e
operator|->
name|d_name
argument_list|,
name|buf
operator|.
name|buf
operator|+
name|dirlen
argument_list|,
name|prefixlen
argument_list|)
condition|)
continue|continue;
name|strbuf_setlen
argument_list|(
operator|&
name|buf
argument_list|,
name|dirlen
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|e
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_pack_on_signal
specifier|static
name|void
name|remove_pack_on_signal
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
name|remove_temporary_files
argument_list|()
expr_stmt|;
name|sigchain_pop
argument_list|(
name|signo
argument_list|)
expr_stmt|;
name|raise
argument_list|(
name|signo
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Adds all packs hex strings to the fname list, which do not  * have a corresponding .keep file.  */
end_comment
begin_function
DECL|function|get_non_kept_pack_filenames
specifier|static
name|void
name|get_non_kept_pack_filenames
parameter_list|(
name|struct
name|string_list
modifier|*
name|fname_list
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|e
decl_stmt|;
name|char
modifier|*
name|fname
decl_stmt|;
name|size_t
name|len
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|dir
operator|=
name|opendir
argument_list|(
name|packdir
argument_list|)
operator|)
condition|)
return|return;
while|while
condition|(
operator|(
name|e
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
operator|!
name|ends_with
argument_list|(
name|e
operator|->
name|d_name
argument_list|,
literal|".pack"
argument_list|)
condition|)
continue|continue;
name|len
operator|=
name|strlen
argument_list|(
name|e
operator|->
name|d_name
argument_list|)
operator|-
name|strlen
argument_list|(
literal|".pack"
argument_list|)
expr_stmt|;
name|fname
operator|=
name|xmemdupz
argument_list|(
name|e
operator|->
name|d_name
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|file_exists
argument_list|(
name|mkpath
argument_list|(
literal|"%s/%s.keep"
argument_list|,
name|packdir
argument_list|,
name|fname
argument_list|)
argument_list|)
condition|)
name|string_list_append_nodup
argument_list|(
name|fname_list
argument_list|,
name|fname
argument_list|)
expr_stmt|;
else|else
name|free
argument_list|(
name|fname
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_redundant_pack
specifier|static
name|void
name|remove_redundant_pack
parameter_list|(
specifier|const
name|char
modifier|*
name|dir_name
parameter_list|,
specifier|const
name|char
modifier|*
name|base_name
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|exts
index|[]
init|=
block|{
literal|".pack"
block|,
literal|".idx"
block|,
literal|".keep"
block|}
decl_stmt|;
name|int
name|i
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|size_t
name|plen
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|buf
argument_list|,
literal|"%s/%s"
argument_list|,
name|dir_name
argument_list|,
name|base_name
argument_list|)
expr_stmt|;
name|plen
operator|=
name|buf
operator|.
name|len
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
name|exts
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
name|strbuf_setlen
argument_list|(
operator|&
name|buf
argument_list|,
name|plen
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|exts
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|ALL_INTO_ONE
define|#
directive|define
name|ALL_INTO_ONE
value|1
end_define
begin_define
DECL|macro|LOOSEN_UNREACHABLE
define|#
directive|define
name|LOOSEN_UNREACHABLE
value|2
end_define
begin_function
DECL|function|cmd_repack
name|int
name|cmd_repack
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
name|exts
index|[
literal|2
index|]
init|=
block|{
literal|".pack"
block|,
literal|".idx"
block|}
decl_stmt|;
name|struct
name|child_process
name|cmd
decl_stmt|;
name|struct
name|string_list_item
modifier|*
name|item
decl_stmt|;
name|struct
name|argv_array
name|cmd_args
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
name|struct
name|string_list
name|names
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|struct
name|string_list
name|rollback
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|struct
name|string_list
name|existing_packs
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|struct
name|strbuf
name|line
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|ext
decl_stmt|,
name|ret
decl_stmt|,
name|failed
decl_stmt|;
name|FILE
modifier|*
name|out
decl_stmt|;
comment|/* variables to be filled by option parsing */
name|int
name|pack_everything
init|=
literal|0
decl_stmt|;
name|int
name|delete_redundant
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|unpack_unreachable
init|=
name|NULL
decl_stmt|;
name|int
name|window
init|=
literal|0
decl_stmt|,
name|window_memory
init|=
literal|0
decl_stmt|;
name|int
name|depth
init|=
literal|0
decl_stmt|;
name|int
name|max_pack_size
init|=
literal|0
decl_stmt|;
name|int
name|no_reuse_delta
init|=
literal|0
decl_stmt|,
name|no_reuse_object
init|=
literal|0
decl_stmt|;
name|int
name|no_update_server_info
init|=
literal|0
decl_stmt|;
name|int
name|quiet
init|=
literal|0
decl_stmt|;
name|int
name|local
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|builtin_repack_options
index|[]
init|=
block|{
name|OPT_BIT
argument_list|(
literal|'a'
argument_list|,
name|NULL
argument_list|,
operator|&
name|pack_everything
argument_list|,
name|N_
argument_list|(
literal|"pack everything in a single pack"
argument_list|)
argument_list|,
name|ALL_INTO_ONE
argument_list|)
block|,
name|OPT_BIT
argument_list|(
literal|'A'
argument_list|,
name|NULL
argument_list|,
operator|&
name|pack_everything
argument_list|,
name|N_
argument_list|(
literal|"same as -a, and turn unreachable objects loose"
argument_list|)
argument_list|,
name|LOOSEN_UNREACHABLE
operator||
name|ALL_INTO_ONE
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'d'
argument_list|,
name|NULL
argument_list|,
operator|&
name|delete_redundant
argument_list|,
name|N_
argument_list|(
literal|"remove redundant packs, and run git-prune-packed"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'f'
argument_list|,
name|NULL
argument_list|,
operator|&
name|no_reuse_delta
argument_list|,
name|N_
argument_list|(
literal|"pass --no-reuse-delta to git-pack-objects"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'F'
argument_list|,
name|NULL
argument_list|,
operator|&
name|no_reuse_object
argument_list|,
name|N_
argument_list|(
literal|"pass --no-reuse-object to git-pack-objects"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'n'
argument_list|,
name|NULL
argument_list|,
operator|&
name|no_update_server_info
argument_list|,
name|N_
argument_list|(
literal|"do not run git-update-server-info"
argument_list|)
argument_list|)
block|,
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
name|N_
argument_list|(
literal|"be quiet"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'l'
argument_list|,
literal|"local"
argument_list|,
operator|&
name|local
argument_list|,
name|N_
argument_list|(
literal|"pass --local to git-pack-objects"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"unpack-unreachable"
argument_list|,
operator|&
name|unpack_unreachable
argument_list|,
name|N_
argument_list|(
literal|"approxidate"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"with -A, do not loosen objects older than this"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"window"
argument_list|,
operator|&
name|window
argument_list|,
name|N_
argument_list|(
literal|"size of the window used for delta compression"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"window-memory"
argument_list|,
operator|&
name|window_memory
argument_list|,
name|N_
argument_list|(
literal|"same as the above, but limit memory size instead of entries count"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
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
literal|"limits the maximum delta depth"
argument_list|)
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"max-pack-size"
argument_list|,
operator|&
name|max_pack_size
argument_list|,
name|N_
argument_list|(
literal|"maximum size of each packfile"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|repack_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
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
name|builtin_repack_options
argument_list|,
name|git_repack_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|packdir
operator|=
name|mkpathdup
argument_list|(
literal|"%s/pack"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|packtmp
operator|=
name|mkpathdup
argument_list|(
literal|"%s/.tmp-%d-pack"
argument_list|,
name|packdir
argument_list|,
operator|(
name|int
operator|)
name|getpid
argument_list|()
argument_list|)
expr_stmt|;
name|sigchain_push_common
argument_list|(
name|remove_pack_on_signal
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"pack-objects"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--keep-true-parents"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--honor-pack-keep"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--non-empty"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--all"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--reflog"
argument_list|)
expr_stmt|;
if|if
condition|(
name|window
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--window=%u"
argument_list|,
name|window
argument_list|)
expr_stmt|;
if|if
condition|(
name|window_memory
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--window-memory=%u"
argument_list|,
name|window_memory
argument_list|)
expr_stmt|;
if|if
condition|(
name|depth
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--depth=%u"
argument_list|,
name|depth
argument_list|)
expr_stmt|;
if|if
condition|(
name|max_pack_size
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--max_pack_size=%u"
argument_list|,
name|max_pack_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|no_reuse_delta
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--no-reuse-delta"
argument_list|)
expr_stmt|;
if|if
condition|(
name|no_reuse_object
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--no-reuse-object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|pack_everything
operator|&
name|ALL_INTO_ONE
condition|)
block|{
name|get_non_kept_pack_filenames
argument_list|(
operator|&
name|existing_packs
argument_list|)
expr_stmt|;
if|if
condition|(
name|existing_packs
operator|.
name|nr
operator|&&
name|delete_redundant
condition|)
block|{
if|if
condition|(
name|unpack_unreachable
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--unpack-unreachable=%s"
argument_list|,
name|unpack_unreachable
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|pack_everything
operator|&
name|LOOSEN_UNREACHABLE
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--unpack-unreachable"
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--unpacked"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--incremental"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|local
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--local"
argument_list|)
expr_stmt|;
if|if
condition|(
name|quiet
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--quiet"
argument_list|)
expr_stmt|;
if|if
condition|(
name|delta_base_offset
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--delta-base-offset"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
name|packtmp
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|cmd
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cmd
argument_list|)
argument_list|)
expr_stmt|;
name|cmd
operator|.
name|argv
operator|=
name|cmd_args
operator|.
name|argv
expr_stmt|;
name|cmd
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|cmd
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|cmd
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
name|ret
operator|=
name|start_command
argument_list|(
operator|&
name|cmd
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
name|out
operator|=
name|xfdopen
argument_list|(
name|cmd
operator|.
name|out
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|line
argument_list|,
name|out
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
condition|)
block|{
if|if
condition|(
name|line
operator|.
name|len
operator|!=
literal|40
condition|)
name|die
argument_list|(
literal|"repack: Expecting 40 character sha1 lines only from pack-objects."
argument_list|)
expr_stmt|;
name|string_list_append
argument_list|(
operator|&
name|names
argument_list|,
name|line
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|out
argument_list|)
expr_stmt|;
name|ret
operator|=
name|finish_command
argument_list|(
operator|&
name|cmd
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
name|argv_array_clear
argument_list|(
operator|&
name|cmd_args
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|names
operator|.
name|nr
operator|&&
operator|!
name|quiet
condition|)
name|printf
argument_list|(
literal|"Nothing new to pack.\n"
argument_list|)
expr_stmt|;
comment|/* 	 * Ok we have prepared all new packfiles. 	 * First see if there are packs of the same name and if so 	 * if we can move them out of the way (this can happen if we 	 * repacked immediately after packing fully. 	 */
name|failed
operator|=
literal|0
expr_stmt|;
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|&names
argument_list|)
block|{
for|for
control|(
name|ext
operator|=
literal|0
init|;
name|ext
operator|<
literal|2
condition|;
name|ext
operator|++
control|)
block|{
name|char
modifier|*
name|fname
decl_stmt|,
modifier|*
name|fname_old
decl_stmt|;
name|fname
operator|=
name|mkpathdup
argument_list|(
literal|"%s/%s%s"
argument_list|,
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|,
name|exts
index|[
name|ext
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|file_exists
argument_list|(
name|fname
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|fname
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|fname_old
operator|=
name|mkpath
argument_list|(
literal|"%s/old-%s%s"
argument_list|,
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|,
name|exts
index|[
name|ext
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|file_exists
argument_list|(
name|fname_old
argument_list|)
condition|)
if|if
condition|(
name|unlink
argument_list|(
name|fname_old
argument_list|)
condition|)
name|failed
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|failed
operator|&&
name|rename
argument_list|(
name|fname
argument_list|,
name|fname_old
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|fname
argument_list|)
expr_stmt|;
name|failed
operator|=
literal|1
expr_stmt|;
break|break;
block|}
else|else
block|{
name|string_list_append
argument_list|(
operator|&
name|rollback
argument_list|,
name|fname
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|failed
condition|)
break|break;
block|}
if|if
condition|(
name|failed
condition|)
block|{
name|struct
name|string_list
name|rollback_failure
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|&rollback
argument_list|)
block|{
name|char
modifier|*
name|fname
decl_stmt|,
modifier|*
name|fname_old
decl_stmt|;
name|fname
operator|=
name|mkpathdup
argument_list|(
literal|"%s/%s"
argument_list|,
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|)
expr_stmt|;
name|fname_old
operator|=
name|mkpath
argument_list|(
literal|"%s/old-%s"
argument_list|,
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|rename
argument_list|(
name|fname_old
argument_list|,
name|fname
argument_list|)
condition|)
name|string_list_append
argument_list|(
operator|&
name|rollback_failure
argument_list|,
name|fname
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|fname
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|rollback_failure
operator|.
name|nr
condition|)
block|{
name|int
name|i
decl_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"WARNING: Some packs in use have been renamed by\n"
literal|"WARNING: prefixing old- to their name, in order to\n"
literal|"WARNING: replace them with the new version of the\n"
literal|"WARNING: file.  But the operation failed, and the\n"
literal|"WARNING: attempt to rename them back to their\n"
literal|"WARNING: original names also failed.\n"
literal|"WARNING: Please rename them in %s manually:\n"
argument_list|,
name|packdir
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
name|rollback_failure
operator|.
name|nr
condition|;
name|i
operator|++
control|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"WARNING:   old-%s -> %s\n"
argument_list|,
name|rollback_failure
operator|.
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|,
name|rollback_failure
operator|.
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
block|}
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
comment|/* Now the ones with the same name are out of the way... */
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|&names
argument_list|)
block|{
for|for
control|(
name|ext
operator|=
literal|0
init|;
name|ext
operator|<
literal|2
condition|;
name|ext
operator|++
control|)
block|{
name|char
modifier|*
name|fname
decl_stmt|,
modifier|*
name|fname_old
decl_stmt|;
name|struct
name|stat
name|statbuffer
decl_stmt|;
name|fname
operator|=
name|mkpathdup
argument_list|(
literal|"%s/pack-%s%s"
argument_list|,
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|,
name|exts
index|[
name|ext
index|]
argument_list|)
expr_stmt|;
name|fname_old
operator|=
name|mkpathdup
argument_list|(
literal|"%s-%s%s"
argument_list|,
name|packtmp
argument_list|,
name|item
operator|->
name|string
argument_list|,
name|exts
index|[
name|ext
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|fname_old
argument_list|,
operator|&
name|statbuffer
argument_list|)
condition|)
block|{
name|statbuffer
operator|.
name|st_mode
operator|&=
operator|~
operator|(
name|S_IWUSR
operator||
name|S_IWGRP
operator||
name|S_IWOTH
operator|)
expr_stmt|;
name|chmod
argument_list|(
name|fname_old
argument_list|,
name|statbuffer
operator|.
name|st_mode
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|rename
argument_list|(
name|fname_old
argument_list|,
name|fname
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"renaming '%s' failed"
argument_list|)
argument_list|,
name|fname_old
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|fname
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|fname_old
argument_list|)
expr_stmt|;
block|}
block|}
comment|/* Remove the "old-" files */
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|&names
argument_list|)
block|{
for|for
control|(
name|ext
operator|=
literal|0
init|;
name|ext
operator|<
literal|2
condition|;
name|ext
operator|++
control|)
block|{
name|char
modifier|*
name|fname
decl_stmt|;
name|fname
operator|=
name|mkpath
argument_list|(
literal|"%s/old-pack-%s%s"
argument_list|,
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|,
name|exts
index|[
name|ext
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|remove_path
argument_list|(
name|fname
argument_list|)
condition|)
name|warning
argument_list|(
name|_
argument_list|(
literal|"removing '%s' failed"
argument_list|)
argument_list|,
name|fname
argument_list|)
expr_stmt|;
block|}
block|}
comment|/* End of pack replacement. */
if|if
condition|(
name|delete_redundant
condition|)
block|{
name|sort_string_list
argument_list|(
operator|&
name|names
argument_list|)
expr_stmt|;
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|&existing_packs
argument_list|)
block|{
name|char
modifier|*
name|sha1
decl_stmt|;
name|size_t
name|len
init|=
name|strlen
argument_list|(
name|item
operator|->
name|string
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<
literal|40
condition|)
continue|continue;
name|sha1
operator|=
name|item
operator|->
name|string
operator|+
name|len
operator|-
literal|40
expr_stmt|;
if|if
condition|(
operator|!
name|string_list_has_string
argument_list|(
operator|&
name|names
argument_list|,
name|sha1
argument_list|)
condition|)
name|remove_redundant_pack
argument_list|(
name|packdir
argument_list|,
name|item
operator|->
name|string
argument_list|)
expr_stmt|;
block|}
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"prune-packed"
argument_list|)
expr_stmt|;
if|if
condition|(
name|quiet
condition|)
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"--quiet"
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|cmd
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cmd
argument_list|)
argument_list|)
expr_stmt|;
name|cmd
operator|.
name|argv
operator|=
name|cmd_args
operator|.
name|argv
expr_stmt|;
name|cmd
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|run_command
argument_list|(
operator|&
name|cmd
argument_list|)
expr_stmt|;
name|argv_array_clear
argument_list|(
operator|&
name|cmd_args
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|no_update_server_info
condition|)
block|{
name|argv_array_push
argument_list|(
operator|&
name|cmd_args
argument_list|,
literal|"update-server-info"
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|cmd
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|cmd
argument_list|)
argument_list|)
expr_stmt|;
name|cmd
operator|.
name|argv
operator|=
name|cmd_args
operator|.
name|argv
expr_stmt|;
name|cmd
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
name|run_command
argument_list|(
operator|&
name|cmd
argument_list|)
expr_stmt|;
name|argv_array_clear
argument_list|(
operator|&
name|cmd_args
argument_list|)
expr_stmt|;
block|}
name|remove_temporary_files
argument_list|()
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|names
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|rollback
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|existing_packs
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|line
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
