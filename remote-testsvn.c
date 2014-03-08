begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"remote.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"url.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"vcs-svn/svndump.h"
end_include
begin_include
include|#
directive|include
file|"notes.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_decl_stmt
DECL|variable|url
specifier|static
specifier|const
name|char
modifier|*
name|url
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|dump_from_file
specifier|static
name|int
name|dump_from_file
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|private_ref
specifier|static
specifier|const
name|char
modifier|*
name|private_ref
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|remote_ref
specifier|static
specifier|const
name|char
modifier|*
name|remote_ref
init|=
literal|"refs/heads/master"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|marksfilename
DECL|variable|notes_ref
specifier|static
specifier|const
name|char
modifier|*
name|marksfilename
decl_stmt|,
modifier|*
name|notes_ref
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|rev_note
DECL|member|rev_nr
struct|struct
name|rev_note
block|{
name|unsigned
name|int
name|rev_nr
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|static
name|int
name|cmd_capabilities
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|int
name|cmd_import
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|int
name|cmd_list
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
function_decl|;
end_function_decl
begin_typedef
DECL|typedef|input_command_handler
typedef|typedef
name|int
function_decl|(
modifier|*
name|input_command_handler
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_typedef
begin_struct
DECL|struct|input_command_entry
struct|struct
name|input_command_entry
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|fn
name|input_command_handler
name|fn
decl_stmt|;
DECL|member|batchable
name|unsigned
name|char
name|batchable
decl_stmt|;
comment|/* whether the command starts or is part of a batch */
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|input_command_list
specifier|static
specifier|const
name|struct
name|input_command_entry
name|input_command_list
index|[]
init|=
block|{
block|{
literal|"capabilities"
block|,
name|cmd_capabilities
block|,
literal|0
block|}
block|,
block|{
literal|"import"
block|,
name|cmd_import
block|,
literal|1
block|}
block|,
block|{
literal|"list"
block|,
name|cmd_list
block|,
literal|0
block|}
block|,
block|{
name|NULL
block|,
name|NULL
block|}
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_capabilities
specifier|static
name|int
name|cmd_capabilities
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|printf
argument_list|(
literal|"import\n"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"bidi-import\n"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"refspec %s:%s\n\n"
argument_list|,
name|remote_ref
argument_list|,
name|private_ref
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|terminate_batch
specifier|static
name|void
name|terminate_batch
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* terminate a current batch's fast-import stream */
name|printf
argument_list|(
literal|"done\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/* NOTE: 'ref' refers to a git reference, while 'rev' refers to a svn revision. */
end_comment
begin_function
DECL|function|read_ref_note
specifier|static
name|char
modifier|*
name|read_ref_note
parameter_list|(
specifier|const
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|note_sha1
decl_stmt|;
name|char
modifier|*
name|msg
init|=
name|NULL
decl_stmt|;
name|unsigned
name|long
name|msglen
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|init_notes
argument_list|(
name|NULL
argument_list|,
name|notes_ref
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|note_sha1
operator|=
name|get_note
argument_list|(
name|NULL
argument_list|,
name|sha1
argument_list|)
operator|)
condition|)
return|return
name|NULL
return|;
comment|/* note tree not found */
if|if
condition|(
operator|!
operator|(
name|msg
operator|=
name|read_sha1_file
argument_list|(
name|note_sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|msglen
argument_list|)
operator|)
condition|)
name|error
argument_list|(
literal|"Empty notes tree. %s"
argument_list|,
name|notes_ref
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|msglen
operator|||
name|type
operator|!=
name|OBJ_BLOB
condition|)
block|{
name|error
argument_list|(
literal|"Note contains unusable content. "
literal|"Is something else using this notes tree? %s"
argument_list|,
name|notes_ref
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|msg
argument_list|)
expr_stmt|;
name|msg
operator|=
name|NULL
expr_stmt|;
block|}
name|free_notes
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
return|return
name|msg
return|;
block|}
end_function
begin_function
DECL|function|parse_rev_note
specifier|static
name|int
name|parse_rev_note
parameter_list|(
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
name|struct
name|rev_note
modifier|*
name|res
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|key
decl_stmt|,
modifier|*
name|value
decl_stmt|,
modifier|*
name|end
decl_stmt|;
name|size_t
name|len
decl_stmt|;
while|while
condition|(
operator|*
name|msg
condition|)
block|{
name|end
operator|=
name|strchrnul
argument_list|(
name|msg
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|len
operator|=
name|end
operator|-
name|msg
expr_stmt|;
name|key
operator|=
literal|"Revision-number: "
expr_stmt|;
if|if
condition|(
name|starts_with
argument_list|(
name|msg
argument_list|,
name|key
argument_list|)
condition|)
block|{
name|long
name|i
decl_stmt|;
name|char
modifier|*
name|end
decl_stmt|;
name|value
operator|=
name|msg
operator|+
name|strlen
argument_list|(
name|key
argument_list|)
expr_stmt|;
name|i
operator|=
name|strtol
argument_list|(
name|value
argument_list|,
operator|&
name|end
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|end
operator|==
name|value
operator|||
name|i
operator|<
literal|0
operator|||
name|i
operator|>
name|UINT32_MAX
condition|)
return|return
operator|-
literal|1
return|;
name|res
operator|->
name|rev_nr
operator|=
name|i
expr_stmt|;
return|return
literal|0
return|;
block|}
name|msg
operator|+=
name|len
operator|+
literal|1
expr_stmt|;
block|}
comment|/* didn't find it */
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|note2mark_cb
specifier|static
name|int
name|note2mark_cb
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|object_sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|note_sha1
parameter_list|,
name|char
modifier|*
name|note_path
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|FILE
modifier|*
name|file
init|=
operator|(
name|FILE
operator|*
operator|)
name|cb_data
decl_stmt|;
name|char
modifier|*
name|msg
decl_stmt|;
name|unsigned
name|long
name|msglen
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|struct
name|rev_note
name|note
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|msg
operator|=
name|read_sha1_file
argument_list|(
name|note_sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|msglen
argument_list|)
operator|)
operator|||
operator|!
name|msglen
operator|||
name|type
operator|!=
name|OBJ_BLOB
condition|)
block|{
name|free
argument_list|(
name|msg
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
name|parse_rev_note
argument_list|(
name|msg
argument_list|,
operator|&
name|note
argument_list|)
condition|)
return|return
literal|2
return|;
if|if
condition|(
name|fprintf
argument_list|(
name|file
argument_list|,
literal|":%d %s\n"
argument_list|,
name|note
operator|.
name|rev_nr
argument_list|,
name|sha1_to_hex
argument_list|(
name|object_sha1
argument_list|)
argument_list|)
operator|<
literal|1
condition|)
return|return
literal|3
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|regenerate_marks
specifier|static
name|void
name|regenerate_marks
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
name|FILE
modifier|*
name|marksfile
init|=
name|fopen
argument_list|(
name|marksfilename
argument_list|,
literal|"w+"
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|marksfile
condition|)
name|die_errno
argument_list|(
literal|"Couldn't create mark file %s."
argument_list|,
name|marksfilename
argument_list|)
expr_stmt|;
name|ret
operator|=
name|for_each_note
argument_list|(
name|NULL
argument_list|,
literal|0
argument_list|,
name|note2mark_cb
argument_list|,
name|marksfile
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
name|die
argument_list|(
literal|"Regeneration of marks failed, returned %d."
argument_list|,
name|ret
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|marksfile
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_or_regenerate_marks
specifier|static
name|void
name|check_or_regenerate_marks
parameter_list|(
name|int
name|latestrev
parameter_list|)
block|{
name|FILE
modifier|*
name|marksfile
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|line
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|found
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|latestrev
operator|<
literal|1
condition|)
return|return;
name|init_notes
argument_list|(
name|NULL
argument_list|,
name|notes_ref
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|marksfile
operator|=
name|fopen
argument_list|(
name|marksfilename
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|marksfile
condition|)
block|{
name|regenerate_marks
argument_list|()
expr_stmt|;
name|marksfile
operator|=
name|fopen
argument_list|(
name|marksfilename
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|marksfile
condition|)
name|die_errno
argument_list|(
literal|"cannot read marks file %s!"
argument_list|,
name|marksfilename
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|marksfile
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|strbuf_addf
argument_list|(
operator|&
name|sb
argument_list|,
literal|":%d "
argument_list|,
name|latestrev
argument_list|)
expr_stmt|;
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|line
argument_list|,
name|marksfile
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
condition|)
block|{
if|if
condition|(
name|starts_with
argument_list|(
name|line
operator|.
name|buf
argument_list|,
name|sb
operator|.
name|buf
argument_list|)
condition|)
block|{
name|found
operator|++
expr_stmt|;
break|break;
block|}
block|}
name|fclose
argument_list|(
name|marksfile
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|found
condition|)
name|regenerate_marks
argument_list|()
expr_stmt|;
block|}
name|free_notes
argument_list|(
name|NULL
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
name|line
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_import
specifier|static
name|int
name|cmd_import
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|int
name|code
decl_stmt|;
name|int
name|dumpin_fd
decl_stmt|;
name|char
modifier|*
name|note_msg
decl_stmt|;
name|unsigned
name|char
name|head_sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|int
name|startrev
decl_stmt|;
name|struct
name|argv_array
name|svndump_argv
init|=
name|ARGV_ARRAY_INIT
decl_stmt|;
name|struct
name|child_process
name|svndump_proc
decl_stmt|;
if|if
condition|(
name|read_ref
argument_list|(
name|private_ref
argument_list|,
name|head_sha1
argument_list|)
condition|)
name|startrev
operator|=
literal|0
expr_stmt|;
else|else
block|{
name|note_msg
operator|=
name|read_ref_note
argument_list|(
name|head_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|note_msg
operator|==
name|NULL
condition|)
block|{
name|warning
argument_list|(
literal|"No note found for %s."
argument_list|,
name|private_ref
argument_list|)
expr_stmt|;
name|startrev
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
name|struct
name|rev_note
name|note
init|=
block|{
literal|0
block|}
decl_stmt|;
if|if
condition|(
name|parse_rev_note
argument_list|(
name|note_msg
argument_list|,
operator|&
name|note
argument_list|)
condition|)
name|die
argument_list|(
literal|"Revision number couldn't be parsed from note."
argument_list|)
expr_stmt|;
name|startrev
operator|=
name|note
operator|.
name|rev_nr
operator|+
literal|1
expr_stmt|;
name|free
argument_list|(
name|note_msg
argument_list|)
expr_stmt|;
block|}
block|}
name|check_or_regenerate_marks
argument_list|(
name|startrev
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|dump_from_file
condition|)
block|{
name|dumpin_fd
operator|=
name|open
argument_list|(
name|url
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|dumpin_fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"Couldn't open svn dump file %s."
argument_list|,
name|url
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|memset
argument_list|(
operator|&
name|svndump_proc
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|child_process
argument_list|)
argument_list|)
expr_stmt|;
name|svndump_proc
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|svndump_argv
argument_list|,
literal|"svnrdump"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|svndump_argv
argument_list|,
literal|"dump"
argument_list|)
expr_stmt|;
name|argv_array_push
argument_list|(
operator|&
name|svndump_argv
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|argv_array_pushf
argument_list|(
operator|&
name|svndump_argv
argument_list|,
literal|"-r%u:HEAD"
argument_list|,
name|startrev
argument_list|)
expr_stmt|;
name|svndump_proc
operator|.
name|argv
operator|=
name|svndump_argv
operator|.
name|argv
expr_stmt|;
name|code
operator|=
name|start_command
argument_list|(
operator|&
name|svndump_proc
argument_list|)
expr_stmt|;
if|if
condition|(
name|code
condition|)
name|die
argument_list|(
literal|"Unable to start %s, code %d"
argument_list|,
name|svndump_proc
operator|.
name|argv
index|[
literal|0
index|]
argument_list|,
name|code
argument_list|)
expr_stmt|;
name|dumpin_fd
operator|=
name|svndump_proc
operator|.
name|out
expr_stmt|;
block|}
comment|/* setup marks file import/export */
name|printf
argument_list|(
literal|"feature import-marks-if-exists=%s\n"
literal|"feature export-marks=%s\n"
argument_list|,
name|marksfilename
argument_list|,
name|marksfilename
argument_list|)
expr_stmt|;
name|svndump_init_fd
argument_list|(
name|dumpin_fd
argument_list|,
name|STDIN_FILENO
argument_list|)
expr_stmt|;
name|svndump_read
argument_list|(
name|url
argument_list|,
name|private_ref
argument_list|,
name|notes_ref
argument_list|)
expr_stmt|;
name|svndump_deinit
argument_list|()
expr_stmt|;
name|svndump_reset
argument_list|()
expr_stmt|;
name|close
argument_list|(
name|dumpin_fd
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dump_from_file
condition|)
block|{
name|code
operator|=
name|finish_command
argument_list|(
operator|&
name|svndump_proc
argument_list|)
expr_stmt|;
if|if
condition|(
name|code
condition|)
name|warning
argument_list|(
literal|"%s, returned %d"
argument_list|,
name|svndump_proc
operator|.
name|argv
index|[
literal|0
index|]
argument_list|,
name|code
argument_list|)
expr_stmt|;
name|argv_array_clear
argument_list|(
operator|&
name|svndump_argv
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_list
specifier|static
name|int
name|cmd_list
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|)
block|{
name|printf
argument_list|(
literal|"? %s\n\n"
argument_list|,
name|remote_ref
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|do_command
specifier|static
name|int
name|do_command
parameter_list|(
name|struct
name|strbuf
modifier|*
name|line
parameter_list|)
block|{
specifier|const
name|struct
name|input_command_entry
modifier|*
name|p
init|=
name|input_command_list
decl_stmt|;
specifier|static
name|struct
name|string_list
name|batchlines
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
specifier|static
specifier|const
name|struct
name|input_command_entry
modifier|*
name|batch_cmd
decl_stmt|;
comment|/* 	 * commands can be grouped together in a batch. 	 * Batches are ended by \n. If no batch is active the program ends. 	 * During a batch all lines are buffered and passed to the handler function 	 * when the batch is terminated. 	 */
if|if
condition|(
name|line
operator|->
name|len
operator|==
literal|0
condition|)
block|{
if|if
condition|(
name|batch_cmd
condition|)
block|{
name|struct
name|string_list_item
modifier|*
name|item
decl_stmt|;
name|for_each_string_list_item
argument_list|(
argument|item
argument_list|,
argument|&batchlines
argument_list|)
name|batch_cmd
operator|->
name|fn
argument_list|(
name|item
operator|->
name|string
argument_list|)
expr_stmt|;
name|terminate_batch
argument_list|()
expr_stmt|;
name|batch_cmd
operator|=
name|NULL
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|batchlines
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
comment|/* end of the batch, continue reading other commands. */
block|}
return|return
literal|1
return|;
comment|/* end of command stream, quit */
block|}
if|if
condition|(
name|batch_cmd
condition|)
block|{
if|if
condition|(
operator|!
name|starts_with
argument_list|(
name|batch_cmd
operator|->
name|name
argument_list|,
name|line
operator|->
name|buf
argument_list|)
condition|)
name|die
argument_list|(
literal|"Active %s batch interrupted by %s"
argument_list|,
name|batch_cmd
operator|->
name|name
argument_list|,
name|line
operator|->
name|buf
argument_list|)
expr_stmt|;
comment|/* buffer batch lines */
name|string_list_append
argument_list|(
operator|&
name|batchlines
argument_list|,
name|line
operator|->
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
for|for
control|(
name|p
operator|=
name|input_command_list
init|;
name|p
operator|->
name|name
condition|;
name|p
operator|++
control|)
block|{
if|if
condition|(
name|starts_with
argument_list|(
name|line
operator|->
name|buf
argument_list|,
name|p
operator|->
name|name
argument_list|)
operator|&&
operator|(
name|strlen
argument_list|(
name|p
operator|->
name|name
argument_list|)
operator|==
name|line
operator|->
name|len
operator|||
name|line
operator|->
name|buf
index|[
name|strlen
argument_list|(
name|p
operator|->
name|name
argument_list|)
index|]
operator|==
literal|' '
operator|)
condition|)
block|{
if|if
condition|(
name|p
operator|->
name|batchable
condition|)
block|{
name|batch_cmd
operator|=
name|p
expr_stmt|;
name|string_list_append
argument_list|(
operator|&
name|batchlines
argument_list|,
name|line
operator|->
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|p
operator|->
name|fn
argument_list|(
name|line
operator|->
name|buf
argument_list|)
return|;
block|}
block|}
name|die
argument_list|(
literal|"Unknown command '%s'\n"
argument_list|,
name|line
operator|->
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
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
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|,
name|url_sb
init|=
name|STRBUF_INIT
decl_stmt|,
name|private_ref_sb
init|=
name|STRBUF_INIT
decl_stmt|,
name|marksfilename_sb
init|=
name|STRBUF_INIT
decl_stmt|,
name|notes_ref_sb
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|static
name|struct
name|remote
modifier|*
name|remote
decl_stmt|;
specifier|const
name|char
modifier|*
name|url_in
decl_stmt|;
name|git_extract_argv0_path
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
if|if
condition|(
name|argc
operator|<
literal|2
operator|||
name|argc
operator|>
literal|3
condition|)
block|{
name|usage
argument_list|(
literal|"git-remote-svn<remote-name> [<url>]"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|remote
operator|=
name|remote_get
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|url_in
operator|=
operator|(
name|argc
operator|==
literal|3
operator|)
condition|?
name|argv
index|[
literal|2
index|]
else|:
name|remote
operator|->
name|url
index|[
literal|0
index|]
expr_stmt|;
if|if
condition|(
name|starts_with
argument_list|(
name|url_in
argument_list|,
literal|"file://"
argument_list|)
condition|)
block|{
name|dump_from_file
operator|=
literal|1
expr_stmt|;
name|url
operator|=
name|url_decode
argument_list|(
name|url_in
operator|+
sizeof|sizeof
argument_list|(
literal|"file://"
argument_list|)
operator|-
literal|1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|dump_from_file
operator|=
literal|0
expr_stmt|;
name|end_url_with_slash
argument_list|(
operator|&
name|url_sb
argument_list|,
name|url_in
argument_list|)
expr_stmt|;
name|url
operator|=
name|url_sb
operator|.
name|buf
expr_stmt|;
block|}
name|strbuf_addf
argument_list|(
operator|&
name|private_ref_sb
argument_list|,
literal|"refs/svn/%s/master"
argument_list|,
name|remote
operator|->
name|name
argument_list|)
expr_stmt|;
name|private_ref
operator|=
name|private_ref_sb
operator|.
name|buf
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|notes_ref_sb
argument_list|,
literal|"refs/notes/%s/revs"
argument_list|,
name|remote
operator|->
name|name
argument_list|)
expr_stmt|;
name|notes_ref
operator|=
name|notes_ref_sb
operator|.
name|buf
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|marksfilename_sb
argument_list|,
literal|"%s/info/fast-import/remote-svn/%s.marks"
argument_list|,
name|get_git_dir
argument_list|()
argument_list|,
name|remote
operator|->
name|name
argument_list|)
expr_stmt|;
name|marksfilename
operator|=
name|marksfilename_sb
operator|.
name|buf
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
if|if
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|==
name|EOF
condition|)
block|{
if|if
condition|(
name|ferror
argument_list|(
name|stdin
argument_list|)
condition|)
name|die
argument_list|(
literal|"Error reading command stream"
argument_list|)
expr_stmt|;
else|else
name|die
argument_list|(
literal|"Unexpected end of command stream"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|do_command
argument_list|(
operator|&
name|buf
argument_list|)
condition|)
break|break;
name|strbuf_reset
argument_list|(
operator|&
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
name|strbuf_release
argument_list|(
operator|&
name|url_sb
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|private_ref_sb
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|notes_ref_sb
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|marksfilename_sb
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
