begin_unit
begin_comment
comment|/*  * Builtin "git diff"  *  * Copyright (c) 2006 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
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
file|"log-tree.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"submodule.h"
end_include
begin_struct
DECL|struct|blobinfo
struct|struct
name|blobinfo
block|{
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|mode
name|unsigned
name|mode
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|builtin_diff_usage
specifier|static
specifier|const
name|char
name|builtin_diff_usage
index|[]
init|=
literal|"git diff<options><rev>{0,2} --<path>*"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|stuff_change
specifier|static
name|void
name|stuff_change
parameter_list|(
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|,
name|unsigned
name|old_mode
parameter_list|,
name|unsigned
name|new_mode
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|old_sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|old_name
parameter_list|,
specifier|const
name|char
modifier|*
name|new_name
parameter_list|)
block|{
name|struct
name|diff_filespec
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
if|if
condition|(
operator|!
name|is_null_sha1
argument_list|(
name|old_sha1
argument_list|)
operator|&&
operator|!
name|is_null_sha1
argument_list|(
name|new_sha1
argument_list|)
operator|&&
operator|!
name|hashcmp
argument_list|(
name|old_sha1
argument_list|,
name|new_sha1
argument_list|)
operator|&&
operator|(
name|old_mode
operator|==
name|new_mode
operator|)
condition|)
return|return;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|REVERSE_DIFF
argument_list|)
condition|)
block|{
name|unsigned
name|tmp
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|tmp_u
decl_stmt|;
specifier|const
name|char
modifier|*
name|tmp_c
decl_stmt|;
name|tmp
operator|=
name|old_mode
expr_stmt|;
name|old_mode
operator|=
name|new_mode
expr_stmt|;
name|new_mode
operator|=
name|tmp
expr_stmt|;
name|tmp_u
operator|=
name|old_sha1
expr_stmt|;
name|old_sha1
operator|=
name|new_sha1
expr_stmt|;
name|new_sha1
operator|=
name|tmp_u
expr_stmt|;
name|tmp_c
operator|=
name|old_name
expr_stmt|;
name|old_name
operator|=
name|new_name
expr_stmt|;
name|new_name
operator|=
name|tmp_c
expr_stmt|;
block|}
if|if
condition|(
name|opt
operator|->
name|prefix
operator|&&
operator|(
name|strncmp
argument_list|(
name|old_name
argument_list|,
name|opt
operator|->
name|prefix
argument_list|,
name|opt
operator|->
name|prefix_length
argument_list|)
operator|||
name|strncmp
argument_list|(
name|new_name
argument_list|,
name|opt
operator|->
name|prefix
argument_list|,
name|opt
operator|->
name|prefix_length
argument_list|)
operator|)
condition|)
return|return;
name|one
operator|=
name|alloc_filespec
argument_list|(
name|old_name
argument_list|)
expr_stmt|;
name|two
operator|=
name|alloc_filespec
argument_list|(
name|new_name
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|one
argument_list|,
name|old_sha1
argument_list|,
name|old_mode
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|two
argument_list|,
name|new_sha1
argument_list|,
name|new_mode
argument_list|)
expr_stmt|;
name|diff_queue
argument_list|(
operator|&
name|diff_queued_diff
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|builtin_diff_b_f
specifier|static
name|int
name|builtin_diff_b_f
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|blobinfo
modifier|*
name|blob
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
comment|/* Blob vs file in the working tree*/
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"failed to stat '%s'"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|||
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"'%s': not a regular file or symlink"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|diff_set_mnemonic_prefix
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
literal|"o/"
argument_list|,
literal|"w/"
argument_list|)
expr_stmt|;
if|if
condition|(
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|==
name|S_IFINVALID
condition|)
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|canon_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
name|stuff_change
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|canon_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|sha1
argument_list|,
name|null_sha1
argument_list|,
name|path
argument_list|,
name|path
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
literal|0
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_blobs
specifier|static
name|int
name|builtin_diff_blobs
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|blobinfo
modifier|*
name|blob
parameter_list|)
block|{
name|unsigned
name|mode
init|=
name|canon_mode
argument_list|(
name|S_IFREG
operator||
literal|0644
argument_list|)
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|==
name|S_IFINVALID
condition|)
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
if|if
condition|(
name|blob
index|[
literal|1
index|]
operator|.
name|mode
operator|==
name|S_IFINVALID
condition|)
name|blob
index|[
literal|1
index|]
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
name|stuff_change
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|blob
index|[
literal|1
index|]
operator|.
name|mode
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|sha1
argument_list|,
name|blob
index|[
literal|1
index|]
operator|.
name|sha1
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|name
argument_list|,
name|blob
index|[
literal|1
index|]
operator|.
name|name
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
literal|0
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_index
specifier|static
name|int
name|builtin_diff_index
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
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
name|int
name|cached
init|=
literal|0
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
condition|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
literal|1
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--cached"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--staged"
argument_list|)
condition|)
name|cached
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|cached
condition|)
name|setup_work_tree
argument_list|()
expr_stmt|;
comment|/* 	 * Make sure there is one revision (i.e. pending object), 	 * and there is no revision filtering parameters. 	 */
if|if
condition|(
name|revs
operator|->
name|pending
operator|.
name|nr
operator|!=
literal|1
operator|||
name|revs
operator|->
name|max_count
operator|!=
operator|-
literal|1
operator|||
name|revs
operator|->
name|min_age
operator|!=
operator|-
literal|1
operator|||
name|revs
operator|->
name|max_age
operator|!=
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_cache_preload
argument_list|(
name|revs
operator|->
name|diffopt
operator|.
name|paths
argument_list|)
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"read_cache_preload"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|run_diff_index
argument_list|(
name|revs
argument_list|,
name|cached
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_tree
specifier|static
name|int
name|builtin_diff_tree
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|object_array_entry
modifier|*
name|ent
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
argument_list|(
name|sha1
index|[
literal|2
index|]
argument_list|)
decl_stmt|;
name|int
name|swap
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
comment|/* We saw two trees, ent[0] and ent[1]. 	 * if ent[1] is uninteresting, they are swapped 	 */
if|if
condition|(
name|ent
index|[
literal|1
index|]
operator|.
name|item
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
name|swap
operator|=
literal|1
expr_stmt|;
name|sha1
index|[
name|swap
index|]
operator|=
name|ent
index|[
literal|0
index|]
operator|.
name|item
operator|->
name|sha1
expr_stmt|;
name|sha1
index|[
literal|1
operator|-
name|swap
index|]
operator|=
name|ent
index|[
literal|1
index|]
operator|.
name|item
operator|->
name|sha1
expr_stmt|;
name|diff_tree_sha1
argument_list|(
name|sha1
index|[
literal|0
index|]
argument_list|,
name|sha1
index|[
literal|1
index|]
argument_list|,
literal|""
argument_list|,
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|revs
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_combined
specifier|static
name|int
name|builtin_diff_combined
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|object_array_entry
modifier|*
name|ent
parameter_list|,
name|int
name|ents
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
argument_list|(
operator|*
name|parent
argument_list|)
decl|[20]
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|dense_combined_merges
operator|&&
operator|!
name|revs
operator|->
name|combine_merges
condition|)
name|revs
operator|->
name|dense_combined_merges
operator|=
name|revs
operator|->
name|combine_merges
operator|=
literal|1
expr_stmt|;
name|parent
operator|=
name|xmalloc
argument_list|(
name|ents
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|parent
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
name|ents
condition|;
name|i
operator|++
control|)
name|hashcpy
argument_list|(
operator|(
name|unsigned
name|char
operator|*
operator|)
operator|(
name|parent
operator|+
name|i
operator|)
argument_list|,
name|ent
index|[
name|i
index|]
operator|.
name|item
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|diff_tree_combined
argument_list|(
name|parent
index|[
literal|0
index|]
argument_list|,
name|parent
operator|+
literal|1
argument_list|,
name|ents
operator|-
literal|1
argument_list|,
name|revs
operator|->
name|dense_combined_merges
argument_list|,
name|revs
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|refresh_index_quietly
specifier|static
name|void
name|refresh_index_quietly
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|lock_file
modifier|*
name|lock_file
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|lock_file
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|lock_file
argument_list|)
argument_list|)
expr_stmt|;
name|fd
operator|=
name|hold_locked_index
argument_list|(
name|lock_file
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return;
name|discard_cache
argument_list|()
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
name|refresh_cache
argument_list|(
name|REFRESH_QUIET
operator||
name|REFRESH_UNMERGED
argument_list|)
expr_stmt|;
if|if
condition|(
name|active_cache_changed
operator|&&
operator|!
name|write_cache
argument_list|(
name|fd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
condition|)
name|commit_locked_index
argument_list|(
name|lock_file
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
name|lock_file
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|builtin_diff_files
specifier|static
name|int
name|builtin_diff_files
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
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
name|int
name|result
decl_stmt|;
name|unsigned
name|int
name|options
init|=
literal|0
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
literal|"--base"
argument_list|)
condition|)
name|revs
operator|->
name|max_count
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
literal|"--ours"
argument_list|)
condition|)
name|revs
operator|->
name|max_count
operator|=
literal|2
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
literal|"--theirs"
argument_list|)
condition|)
name|revs
operator|->
name|max_count
operator|=
literal|3
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
name|options
operator||=
name|DIFF_SILENT_ON_REMOVED
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
literal|"-h"
argument_list|)
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
else|else
return|return
name|error
argument_list|(
literal|"invalid option: %s"
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
comment|/* 	 * "diff --base" should not combine merges because it was not 	 * asked to.  "diff -c" should not densify (if the user wants 	 * dense one, --cc can be explicitly asked for, or just rely 	 * on the default). 	 */
if|if
condition|(
name|revs
operator|->
name|max_count
operator|==
operator|-
literal|1
operator|&&
operator|!
name|revs
operator|->
name|combine_merges
operator|&&
operator|(
name|revs
operator|->
name|diffopt
operator|.
name|output_format
operator|&
name|DIFF_FORMAT_PATCH
operator|)
condition|)
name|revs
operator|->
name|combine_merges
operator|=
name|revs
operator|->
name|dense_combined_merges
operator|=
literal|1
expr_stmt|;
name|setup_work_tree
argument_list|()
expr_stmt|;
if|if
condition|(
name|read_cache_preload
argument_list|(
name|revs
operator|->
name|diffopt
operator|.
name|paths
argument_list|)
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"read_cache_preload"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|result
operator|=
name|run_diff_files
argument_list|(
name|revs
argument_list|,
name|options
argument_list|)
expr_stmt|;
return|return
name|diff_result_code
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|result
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cmd_diff
name|int
name|cmd_diff
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
name|rev_info
name|rev
decl_stmt|;
name|struct
name|object_array_entry
name|ent
index|[
literal|100
index|]
decl_stmt|;
name|int
name|ents
init|=
literal|0
decl_stmt|,
name|blobs
init|=
literal|0
decl_stmt|,
name|paths
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
init|=
name|NULL
decl_stmt|;
name|struct
name|blobinfo
name|blob
index|[
literal|2
index|]
decl_stmt|;
name|int
name|nongit
decl_stmt|;
name|int
name|result
init|=
literal|0
decl_stmt|;
comment|/* 	 * We could get N tree-ish in the rev.pending_objects list. 	 * Also there could be M blobs there, and P pathspecs. 	 * 	 * N=0, M=0: 	 *	cache vs files (diff-files) 	 * N=0, M=2: 	 *      compare two random blobs.  P must be zero. 	 * N=0, M=1, P=1: 	 *	compare a blob with a working tree file. 	 * 	 * N=1, M=0: 	 *      tree vs cache (diff-index --cached) 	 * 	 * N=2, M=0: 	 *      tree vs tree (diff-tree) 	 * 	 * N=0, M=0, P=2: 	 *      compare two filesystem entities (aka --no-index). 	 * 	 * Other cases are errors. 	 */
name|prefix
operator|=
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
expr_stmt|;
name|gitmodules_config
argument_list|()
expr_stmt|;
name|git_config
argument_list|(
name|git_diff_ui_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_use_color_default
operator|==
operator|-
literal|1
condition|)
name|diff_use_color_default
operator|=
name|git_use_color_default
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
comment|/* If this is a no-index diff, just run it and exit there. */
name|diff_no_index
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|nongit
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
comment|/* Otherwise, we are doing the usual "git" diff */
name|rev
operator|.
name|diffopt
operator|.
name|skip_stat_unmatch
operator|=
operator|!
operator|!
name|diff_auto_refresh_index
expr_stmt|;
comment|/* Default to let external and textconv be used */
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|ALLOW_EXTERNAL
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|ALLOW_TEXTCONV
argument_list|)
expr_stmt|;
if|if
condition|(
name|nongit
condition|)
name|die
argument_list|(
literal|"Not a git repository"
argument_list|)
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rev
operator|.
name|diffopt
operator|.
name|output_format
condition|)
block|{
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_PATCH
expr_stmt|;
if|if
condition|(
name|diff_setup_done
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"diff_setup_done failed"
argument_list|)
expr_stmt|;
block|}
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|RECURSIVE
argument_list|)
expr_stmt|;
comment|/* 	 * If the user asked for our exit code then don't start a 	 * pager or we would end up reporting its exit code instead. 	 */
if|if
condition|(
operator|!
name|DIFF_OPT_TST
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|EXIT_WITH_STATUS
argument_list|)
operator|&&
name|check_pager_config
argument_list|(
literal|"diff"
argument_list|)
operator|!=
literal|0
condition|)
name|setup_pager
argument_list|()
expr_stmt|;
comment|/* 	 * Do we have --cached and not have a pending object, then 	 * default to HEAD by hand.  Eek. 	 */
if|if
condition|(
operator|!
name|rev
operator|.
name|pending
operator|.
name|nr
condition|)
block|{
name|int
name|i
decl_stmt|;
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
literal|"--"
argument_list|)
condition|)
break|break;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--cached"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--staged"
argument_list|)
condition|)
block|{
name|add_head_to_pending
argument_list|(
operator|&
name|rev
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rev
operator|.
name|pending
operator|.
name|nr
condition|)
name|die
argument_list|(
literal|"No HEAD commit to compare with (yet)"
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|rev
operator|.
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_array_entry
modifier|*
name|list
init|=
name|rev
operator|.
name|pending
operator|.
name|objects
operator|+
name|i
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
name|list
operator|->
name|item
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|list
operator|->
name|name
decl_stmt|;
name|int
name|flags
init|=
operator|(
name|obj
operator|->
name|flags
operator|&
name|UNINTERESTING
operator|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
operator|->
name|parsed
condition|)
name|obj
operator|=
name|parse_object
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|obj
operator|=
name|deref_tag
argument_list|(
name|obj
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
name|die
argument_list|(
literal|"invalid object '%s' given."
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
name|obj
operator|=
operator|&
operator|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
operator|)
operator|->
name|tree
operator|->
name|object
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TREE
condition|)
block|{
if|if
condition|(
name|ARRAY_SIZE
argument_list|(
name|ent
argument_list|)
operator|<=
name|ents
condition|)
name|die
argument_list|(
literal|"more than %d trees given: '%s'"
argument_list|,
operator|(
name|int
operator|)
name|ARRAY_SIZE
argument_list|(
name|ent
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|obj
operator|->
name|flags
operator||=
name|flags
expr_stmt|;
name|ent
index|[
name|ents
index|]
operator|.
name|item
operator|=
name|obj
expr_stmt|;
name|ent
index|[
name|ents
index|]
operator|.
name|name
operator|=
name|name
expr_stmt|;
name|ents
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_BLOB
condition|)
block|{
if|if
condition|(
literal|2
operator|<=
name|blobs
condition|)
name|die
argument_list|(
literal|"more than two blobs given: '%s'"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|blob
index|[
name|blobs
index|]
operator|.
name|sha1
argument_list|,
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
name|blob
index|[
name|blobs
index|]
operator|.
name|name
operator|=
name|name
expr_stmt|;
name|blob
index|[
name|blobs
index|]
operator|.
name|mode
operator|=
name|list
operator|->
name|mode
expr_stmt|;
name|blobs
operator|++
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"unhandled object '%s' given."
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|rev
operator|.
name|prune_data
condition|)
block|{
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
init|=
name|rev
operator|.
name|prune_data
decl_stmt|;
while|while
condition|(
operator|*
name|pathspec
condition|)
block|{
if|if
condition|(
operator|!
name|path
condition|)
name|path
operator|=
operator|*
name|pathspec
expr_stmt|;
name|paths
operator|++
expr_stmt|;
name|pathspec
operator|++
expr_stmt|;
block|}
block|}
comment|/* 	 * Now, do the arguments look reasonable? 	 */
if|if
condition|(
operator|!
name|ents
condition|)
block|{
switch|switch
condition|(
name|blobs
condition|)
block|{
case|case
literal|0
case|:
name|result
operator|=
name|builtin_diff_files
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
break|break;
case|case
literal|1
case|:
if|if
condition|(
name|paths
operator|!=
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|result
operator|=
name|builtin_diff_b_f
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|blob
argument_list|,
name|path
argument_list|)
expr_stmt|;
break|break;
case|case
literal|2
case|:
if|if
condition|(
name|paths
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|result
operator|=
name|builtin_diff_blobs
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|blob
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|blobs
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ents
operator|==
literal|1
condition|)
name|result
operator|=
name|builtin_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ents
operator|==
literal|2
condition|)
name|result
operator|=
name|builtin_diff_tree
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|ent
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ent
index|[
literal|0
index|]
operator|.
name|item
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
block|{
comment|/* 		 * diff A...B where there is at least one merge base 		 * between A and B.  We have ent[0] == merge-base, 		 * ent[ents-2] == A, and ent[ents-1] == B.  Show diff 		 * between the base and B.  Note that we pick one 		 * merge base at random if there are more than one. 		 */
name|ent
index|[
literal|1
index|]
operator|=
name|ent
index|[
name|ents
operator|-
literal|1
index|]
expr_stmt|;
name|result
operator|=
name|builtin_diff_tree
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|ent
argument_list|)
expr_stmt|;
block|}
else|else
name|result
operator|=
name|builtin_diff_combined
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|ent
argument_list|,
name|ents
argument_list|)
expr_stmt|;
name|result
operator|=
name|diff_result_code
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|result
argument_list|)
expr_stmt|;
if|if
condition|(
literal|1
operator|<
name|rev
operator|.
name|diffopt
operator|.
name|skip_stat_unmatch
condition|)
name|refresh_index_quietly
argument_list|()
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
end_unit
