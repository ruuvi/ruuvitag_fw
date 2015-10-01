#
# some common cmake tools
#

#############################################################################
#
# Copyright (C) 2015, Offcode Ltd. All rights reserved.
# Author: Janne Rosberg <janne@offcode.fi>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice,   this list of conditions and the following disclaimer.
#    * Redistributions in  binary form must  reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the RuuviTag nor the
#      names of its contributors may be used to endorse or promote products
#      derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND  ANY  EXPRESS  OR  IMPLIED WARRANTIES,  INCLUDING,  BUT NOT LIMITED TO,
# THE  IMPLIED  WARRANTIES  OF MERCHANTABILITY  AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR
# CONSEQUENTIAL  DAMAGES  (INCLUDING,  BUT  NOT  LIMITED  TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES;  LOSS OF USE, DATA, OR PROFITS;  OR BUSINESS
# INTERRUPTION)  HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,  WHETHER IN
# CONTRACT,  STRICT LIABILITY,  OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#############################################################################

#--------
# ADD_LINK_FLAGS (<target> "flags...")
#
MACRO (ADD_LINK_FLAGS _target _flg)
  get_target_property(_flags ${_target} LINK_FLAGS)
  if (_flags)
    set(_flags "${_flags} ${_flg}")
  else (_flags)
    set(_flags "${_flg}")
  endif (_flags)
  set_target_properties(${_target} PROPERTIES LINK_FLAGS "${_flags}")

ENDMACRO(ADD_LINK_FLAGS)

#
# create_bin_from (<target>)
#
# Create binary (with objcopy) from target elf.
#
function(create_bin_from target)
  if (TARGET ${target})
	add_custom_command(TARGET ${target}
			   POST_BUILD
			   COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.bin
			   COMMENT "Post Build BIN " ${target}
			   VERBATIM)
  else()
	message (AUTHOR_WARNING "${target} is NOT a target.")
  endif()
endfunction()

#
# create_hex_from (<target>)
#
# Create intel-HEX file (with objcopy) from target elf.
#
function(create_hex_from target)
  if (TARGET ${target})
	add_custom_command(TARGET ${target}
			POST_BUILD
			COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.hex
			COMMENT "Post Build HEX " ${target}
			VERBATIM)
  else()
	message (AUTHOR_WARNING "${target} is NOT a target.")
  endif()
endfunction()

