begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|BUILTIN_H
end_ifndef
begin_define
DECL|macro|BUILTIN_H
define|#
directive|define
name|BUILTIN_H
end_define
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_ifndef
ifndef|#
directive|ifndef
name|PATH_MAX
end_ifndef
begin_define
DECL|macro|PATH_MAX
define|#
directive|define
name|PATH_MAX
value|4096
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_decl_stmt
specifier|extern
specifier|const
name|char
name|git_version_string
index|[]
decl_stmt|;
end_decl_stmt
begin_function_decl
name|void
name|cmd_usage
parameter_list|(
name|int
name|show_all
parameter_list|,
specifier|const
name|char
modifier|*
name|exec_path
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
ifdef|#
directive|ifdef
name|__GNUC__
function_decl|__attribute__
parameter_list|(
function_decl|(__format__
parameter_list|(
name|__printf__
parameter_list|,
function_decl|3
operator|,
function_decl|4
end_function_decl
begin_operator
unit|)
operator|,
end_operator
begin_expr_stmt
name|__noreturn__
end_expr_stmt
begin_endif
unit|))
endif|#
directive|endif
end_endif
begin_empty_stmt
empty_stmt|;
end_empty_stmt
begin_function_decl
specifier|extern
name|int
name|cmd_help
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_version
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_whatchanged
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_show
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_log
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
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
name|char
modifier|*
modifier|*
name|envp
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_format_patch
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_count_objects
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_prune
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_push
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_grep
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_rm
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_add
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_rev_list
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_check_ref_format
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_init_db
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_tar_tree
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_upload_tar
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_get_tar_commit_id
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_ls_files
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_ls_tree
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_read_tree
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_commit_tree
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_apply
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_show_branch
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_diff_files
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_diff_index
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_diff_stages
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_diff_tree
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_cat_file
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_rev_parse
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_update_index
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_update_ref
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_fmt_merge_msg
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_write_tree
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|write_tree
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|missing_ok
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_mailsplit
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|split_mbox
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|mbox
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|,
name|int
name|allow_bare
parameter_list|,
name|int
name|nr_prec
parameter_list|,
name|int
name|skip
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_mailinfo
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|mailinfo
parameter_list|(
name|FILE
modifier|*
name|in
parameter_list|,
name|FILE
modifier|*
name|out
parameter_list|,
name|int
name|ks
parameter_list|,
specifier|const
name|char
modifier|*
name|encoding
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
specifier|const
name|char
modifier|*
name|patch
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|cmd_stripspace
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
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|stripspace
parameter_list|(
name|FILE
modifier|*
name|in
parameter_list|,
name|FILE
modifier|*
name|out
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
