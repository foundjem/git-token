begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|STATUS_H
end_ifndef
begin_define
DECL|macro|STATUS_H
define|#
directive|define
name|STATUS_H
end_define
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"pathspec.h"
end_include
begin_struct_decl
struct_decl|struct
name|worktree
struct_decl|;
end_struct_decl
begin_enum
DECL|enum|color_wt_status
enum|enum
name|color_wt_status
block|{
DECL|enumerator|WT_STATUS_HEADER
name|WT_STATUS_HEADER
init|=
literal|0
block|,
DECL|enumerator|WT_STATUS_UPDATED
name|WT_STATUS_UPDATED
block|,
DECL|enumerator|WT_STATUS_CHANGED
name|WT_STATUS_CHANGED
block|,
DECL|enumerator|WT_STATUS_UNTRACKED
name|WT_STATUS_UNTRACKED
block|,
DECL|enumerator|WT_STATUS_NOBRANCH
name|WT_STATUS_NOBRANCH
block|,
DECL|enumerator|WT_STATUS_UNMERGED
name|WT_STATUS_UNMERGED
block|,
DECL|enumerator|WT_STATUS_LOCAL_BRANCH
name|WT_STATUS_LOCAL_BRANCH
block|,
DECL|enumerator|WT_STATUS_REMOTE_BRANCH
name|WT_STATUS_REMOTE_BRANCH
block|,
DECL|enumerator|WT_STATUS_ONBRANCH
name|WT_STATUS_ONBRANCH
block|,
DECL|enumerator|WT_STATUS_MAXSLOT
name|WT_STATUS_MAXSLOT
block|}
enum|;
end_enum
begin_enum
DECL|enum|untracked_status_type
enum|enum
name|untracked_status_type
block|{
DECL|enumerator|SHOW_NO_UNTRACKED_FILES
name|SHOW_NO_UNTRACKED_FILES
block|,
DECL|enumerator|SHOW_NORMAL_UNTRACKED_FILES
name|SHOW_NORMAL_UNTRACKED_FILES
block|,
DECL|enumerator|SHOW_ALL_UNTRACKED_FILES
name|SHOW_ALL_UNTRACKED_FILES
block|}
enum|;
end_enum
begin_comment
comment|/* from where does this commit originate */
end_comment
begin_enum
DECL|enum|commit_whence
enum|enum
name|commit_whence
block|{
DECL|enumerator|FROM_COMMIT
name|FROM_COMMIT
block|,
comment|/* normal */
DECL|enumerator|FROM_MERGE
name|FROM_MERGE
block|,
comment|/* commit came from merge */
DECL|enumerator|FROM_CHERRY_PICK
name|FROM_CHERRY_PICK
comment|/* commit came from cherry-pick */
block|}
enum|;
end_enum
begin_struct
DECL|struct|wt_status_change_data
struct|struct
name|wt_status_change_data
block|{
DECL|member|worktree_status
name|int
name|worktree_status
decl_stmt|;
DECL|member|index_status
name|int
name|index_status
decl_stmt|;
DECL|member|stagemask
name|int
name|stagemask
decl_stmt|;
DECL|member|head_path
name|char
modifier|*
name|head_path
decl_stmt|;
DECL|member|dirty_submodule
name|unsigned
name|dirty_submodule
range|:
literal|2
decl_stmt|;
DECL|member|new_submodule_commits
name|unsigned
name|new_submodule_commits
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|wt_status
struct|struct
name|wt_status
block|{
DECL|member|is_initial
name|int
name|is_initial
decl_stmt|;
DECL|member|branch
name|char
modifier|*
name|branch
decl_stmt|;
DECL|member|reference
specifier|const
name|char
modifier|*
name|reference
decl_stmt|;
DECL|member|pathspec
name|struct
name|pathspec
name|pathspec
decl_stmt|;
DECL|member|verbose
name|int
name|verbose
decl_stmt|;
DECL|member|amend
name|int
name|amend
decl_stmt|;
DECL|member|whence
name|enum
name|commit_whence
name|whence
decl_stmt|;
DECL|member|nowarn
name|int
name|nowarn
decl_stmt|;
DECL|member|use_color
name|int
name|use_color
decl_stmt|;
DECL|member|no_gettext
name|int
name|no_gettext
decl_stmt|;
DECL|member|display_comment_prefix
name|int
name|display_comment_prefix
decl_stmt|;
DECL|member|relative_paths
name|int
name|relative_paths
decl_stmt|;
DECL|member|submodule_summary
name|int
name|submodule_summary
decl_stmt|;
DECL|member|show_ignored_files
name|int
name|show_ignored_files
decl_stmt|;
DECL|member|show_untracked_files
name|enum
name|untracked_status_type
name|show_untracked_files
decl_stmt|;
DECL|member|ignore_submodule_arg
specifier|const
name|char
modifier|*
name|ignore_submodule_arg
decl_stmt|;
DECL|member|color_palette
name|char
name|color_palette
index|[
name|WT_STATUS_MAXSLOT
index|]
index|[
name|COLOR_MAXLEN
index|]
decl_stmt|;
DECL|member|colopts
name|unsigned
name|colopts
decl_stmt|;
DECL|member|null_termination
name|int
name|null_termination
decl_stmt|;
DECL|member|show_branch
name|int
name|show_branch
decl_stmt|;
DECL|member|hints
name|int
name|hints
decl_stmt|;
comment|/* These are computed during processing of the individual sections */
DECL|member|commitable
name|int
name|commitable
decl_stmt|;
DECL|member|workdir_dirty
name|int
name|workdir_dirty
decl_stmt|;
DECL|member|index_file
specifier|const
name|char
modifier|*
name|index_file
decl_stmt|;
DECL|member|fp
name|FILE
modifier|*
name|fp
decl_stmt|;
DECL|member|prefix
specifier|const
name|char
modifier|*
name|prefix
decl_stmt|;
DECL|member|change
name|struct
name|string_list
name|change
decl_stmt|;
DECL|member|untracked
name|struct
name|string_list
name|untracked
decl_stmt|;
DECL|member|ignored
name|struct
name|string_list
name|ignored
decl_stmt|;
DECL|member|untracked_in_ms
name|uint32_t
name|untracked_in_ms
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|wt_status_state
struct|struct
name|wt_status_state
block|{
DECL|member|merge_in_progress
name|int
name|merge_in_progress
decl_stmt|;
DECL|member|am_in_progress
name|int
name|am_in_progress
decl_stmt|;
DECL|member|am_empty_patch
name|int
name|am_empty_patch
decl_stmt|;
DECL|member|rebase_in_progress
name|int
name|rebase_in_progress
decl_stmt|;
DECL|member|rebase_interactive_in_progress
name|int
name|rebase_interactive_in_progress
decl_stmt|;
DECL|member|cherry_pick_in_progress
name|int
name|cherry_pick_in_progress
decl_stmt|;
DECL|member|bisect_in_progress
name|int
name|bisect_in_progress
decl_stmt|;
DECL|member|revert_in_progress
name|int
name|revert_in_progress
decl_stmt|;
DECL|member|detached_at
name|int
name|detached_at
decl_stmt|;
DECL|member|branch
name|char
modifier|*
name|branch
decl_stmt|;
DECL|member|onto
name|char
modifier|*
name|onto
decl_stmt|;
DECL|member|detached_from
name|char
modifier|*
name|detached_from
decl_stmt|;
DECL|member|detached_sha1
name|unsigned
name|char
name|detached_sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|revert_head_sha1
name|unsigned
name|char
name|revert_head_sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|cherry_pick_head_sha1
name|unsigned
name|char
name|cherry_pick_head_sha1
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|void
name|wt_status_truncate_message_at_cut_line
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_status_add_cut_line
parameter_list|(
name|FILE
modifier|*
name|fp
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_status_prepare
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_status_print
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_status_collect
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_status_get_state
parameter_list|(
name|struct
name|wt_status_state
modifier|*
name|state
parameter_list|,
name|int
name|get_detached_from
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|wt_status_check_rebase
parameter_list|(
specifier|const
name|struct
name|worktree
modifier|*
name|wt
parameter_list|,
name|struct
name|wt_status_state
modifier|*
name|state
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|wt_status_check_bisect
parameter_list|(
specifier|const
name|struct
name|worktree
modifier|*
name|wt
parameter_list|,
name|struct
name|wt_status_state
modifier|*
name|state
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_shortstatus_print
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|wt_porcelain_print
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|)
function_decl|;
end_function_decl
begin_macro
name|__attribute__
argument_list|(
argument|(format (printf,
literal|3
argument|,
literal|4
argument|))
argument_list|)
end_macro
begin_function_decl
name|void
name|status_printf_ln
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|,
specifier|const
name|char
modifier|*
name|color
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_macro
name|__attribute__
argument_list|(
argument|(format (printf,
literal|3
argument|,
literal|4
argument|))
argument_list|)
end_macro
begin_function_decl
name|void
name|status_printf
parameter_list|(
name|struct
name|wt_status
modifier|*
name|s
parameter_list|,
specifier|const
name|char
modifier|*
name|color
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* STATUS_H */
end_comment
end_unit
