
""" Python equivalent of akai section multitools

Methods to manipulate multis
"""

__author__ =  'Walco van Loon'
__version__=  '0.1'

import aksy.sysex

class Multitools:
     def __init__(self, z48):
          self.z48 = z48
          self.commands = {}
          self.command_spec = aksy.sysex.CommandSpec('\x47\x5f\x00', aksy.sysex.CommandSpec.ID, aksy.sysex.CommandSpec.ARGS)
          comm = aksy.sysex.Command('\x18\x01', 'get_no_items', (), (aksy.sysex.BYTE,))
          self.commands['\x18\x01'] = comm
          comm = aksy.sysex.Command('\x18\x02\x00', 'get_handles', (), (aksy.sysex.DWORD,))
          self.commands['\x18\x02\x00'] = comm
          comm = aksy.sysex.Command('\x18\x02\x01', 'get_names', (), (aksy.sysex.STRING,))
          self.commands['\x18\x02\x01'] = comm
          comm = aksy.sysex.Command('\x18\x02\x02', 'get_handles_names', (), (aksy.sysex.DWORD, aksy.sysex.STRING))
          self.commands['\x18\x02\x02'] = comm
          comm = aksy.sysex.Command('\x18\x02\x03', 'get_handles_tagged', (), (aksy.sysex.DWORD, aksy.sysex.STRING))
          self.commands['\x18\x02\x03'] = comm
          comm = aksy.sysex.Command('\x18\x03', 'set_current_by_handle', (aksy.sysex.DWORD,), ())
          self.commands['\x18\x03'] = comm
          comm = aksy.sysex.Command('\x18\x04', 'set_current_by_name', (aksy.sysex.STRING,), ())
          self.commands['\x18\x04'] = comm
          comm = aksy.sysex.Command('\x18\x05', 'get_current_handle', (), (aksy.sysex.DWORD,))
          self.commands['\x18\x05'] = comm
          comm = aksy.sysex.Command('\x18\x06', 'get_current_name', (), (aksy.sysex.STRING,))
          self.commands['\x18\x06'] = comm
          comm = aksy.sysex.Command('\x18\x07', 'get_name_by_handle', (aksy.sysex.DWORD,), (aksy.sysex.STRING,))
          self.commands['\x18\x07'] = comm
          comm = aksy.sysex.Command('\x18\x08', 'get_handle_by_name', (aksy.sysex.STRING,), (aksy.sysex.DWORD,))
          self.commands['\x18\x08'] = comm
          comm = aksy.sysex.Command('\x18\x09', 'delete_all', (), ())
          self.commands['\x18\x09'] = comm
          comm = aksy.sysex.Command('\x18\x0A', 'delete_current', (), ())
          self.commands['\x18\x0A'] = comm
          comm = aksy.sysex.Command('\x18\x0B', 'delete_by_handle', (aksy.sysex.DWORD,), ())
          self.commands['\x18\x0B'] = comm
          comm = aksy.sysex.Command('\x18\x0C', 'rename_current', (aksy.sysex.STRING,), ())
          self.commands['\x18\x0C'] = comm
          comm = aksy.sysex.Command('\x18\x0D', 'rename_by_handle', (aksy.sysex.DWORD, aksy.sysex.STRING), ())
          self.commands['\x18\x0D'] = comm
          comm = aksy.sysex.Command('\x18\x0E', 'tag', (aksy.sysex.BYTE, aksy.sysex.BYTE, aksy.sysex.BYTE), ())
          self.commands['\x18\x0E'] = comm
          comm = aksy.sysex.Command('\x18\x0F', 'get_tag_bitmap', (), (aksy.sysex.WORD,))
          self.commands['\x18\x0F'] = comm
          comm = aksy.sysex.Command('\x18\x10', 'get_current_modified', (), (aksy.sysex.STRING,))
          self.commands['\x18\x10'] = comm
          comm = aksy.sysex.Command('\x1B\x01', 'get_group_id', (), (aksy.sysex.BYTE,))
          self.commands['\x1B\x01'] = comm
          comm = aksy.sysex.Command('\x1B\x02', 'get_multi_select_method', (), (aksy.sysex.BYTE,))
          self.commands['\x1B\x02'] = comm
          comm = aksy.sysex.Command('\x1B\x03', 'get_multi_select_channel', (), (aksy.sysex.BYTE,))
          self.commands['\x1B\x03'] = comm
          comm = aksy.sysex.Command('\x1B\x04', 'get_multi_tempo', (), (aksy.sysex.WORD,))
          self.commands['\x1B\x04'] = comm
          comm = aksy.sysex.Command('\x1B\x08', 'get_multi_program_no', (), (aksy.sysex.WORD,))
          self.commands['\x1B\x08'] = comm
          comm = aksy.sysex.Command('\x1B\x09', 'get_multi_part_handle', (aksy.sysex.DWORD,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x09'] = comm
          comm = aksy.sysex.Command('\x1B\x0A', 'get_multi_part_name', (aksy.sysex.BYTE,), (aksy.sysex.STRING,))
          self.commands['\x1B\x0A'] = comm
          comm = aksy.sysex.Command('\x1B\x0F', 'get_no_parts', (), (aksy.sysex.WORD,))
          self.commands['\x1B\x0F'] = comm
          comm = aksy.sysex.Command('\x1B\x10', 'get_part_midi_channel', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x10'] = comm
          comm = aksy.sysex.Command('\x1B\x11', 'get_part_mute', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x11'] = comm
          comm = aksy.sysex.Command('\x1B\x12', 'get_part_solo', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x12'] = comm
          comm = aksy.sysex.Command('\x1B\x13', 'get_part_level', (), (aksy.sysex.SWORD,))
          self.commands['\x1B\x13'] = comm
          comm = aksy.sysex.Command('\x1B\x14', 'get_part_output', (aksy.sysex.= L/R; 1�4 = op1/2�op7/8; 5�14 = L, R, op1-op8), aksy.sysex.BYTE), (aksy.sysex.BYTE,))
          self.commands['\x1B\x14'] = comm
          comm = aksy.sysex.Command('\x1B\x15', 'get_part_pan', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x15'] = comm
          comm = aksy.sysex.Command('\x1B\x16', 'get_part_fx_channel', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x16'] = comm
          comm = aksy.sysex.Command('\x1B\x17', 'get_part_fx_send_level', (), (aksy.sysex.SWORD,))
          self.commands['\x1B\x17'] = comm
          comm = aksy.sysex.Command('\x1B\x18', 'get_part_tune', (aksy.sysex.BYTE,), (aksy.sysex.SWORD,))
          self.commands['\x1B\x18'] = comm
          comm = aksy.sysex.Command('\x1B\x1A', 'get_part_low_note', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x1A'] = comm
          comm = aksy.sysex.Command('\x1B\x1B', 'get_part_high_note', (aksy.sysex.BYTE,), (aksy.sysex.BYTE,))
          self.commands['\x1B\x1B'] = comm
          comm = aksy.sysex.Command('\x1A\x01', 'set_group_id', (aksy.sysex.BYTE,), ())
          self.commands['\x1A\x01'] = comm
          comm = aksy.sysex.Command('\x1A\x02', 'set_multi_select_method', (aksy.sysex.BYTE,), ())
          self.commands['\x1A\x02'] = comm
          comm = aksy.sysex.Command('\x1A\x03', 'set_multi_select_channel', (aksy.sysex.BYTE,), ())
          self.commands['\x1A\x03'] = comm
          comm = aksy.sysex.Command('\x1A\x04', 'set_multi_tempo', (aksy.sysex.WORD,), ())
          self.commands['\x1A\x04'] = comm
          comm = aksy.sysex.Command('\x1A\x08', 'set_multi_program_no', (aksy.sysex.BYTE,), ())
          self.commands['\x1A\x08'] = comm
          comm = aksy.sysex.Command('\x1A\x09', 'set_multi_part_by_handle', (aksy.sysex.BYTE, aksy.sysex.DWORD), ())
          self.commands['\x1A\x09'] = comm

     def get_no_items(self):
          """Get number of items in memory

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x18\x01')
          return self.z48.execute(comm, ())

     def get_handles(self):
          """Get handles <Data1>: 0=list of handles

          Returns:
               aksy.sysex.DWORD
          """
          comm = self.commands.get('\x18\x02\x00')
          return self.z48.execute(comm, ())

     def get_names(self):
          """Get names items: <Data1>; 1=list of names;

          Returns:
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x18\x02\x01')
          return self.z48.execute(comm, ())

     def get_handles_names(self):
          """Get handles names: <Data1>; 2=list of handle+name

          Returns:
               aksy.sysex.DWORD
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x18\x02\x02')
          return self.z48.execute(comm, ())

     def get_handles_tagged(self):
          """Get handles tagged <Data1> ; 3=list of handle+modified/tagged name

          Returns:
               aksy.sysex.DWORD
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x18\x02\x03')
          return self.z48.execute(comm, ())

     def set_current_by_handle(self, arg0):
          """Select by handle
          """
          comm = self.commands.get('\x18\x03')
          return self.z48.execute(comm, (arg0, ))

     def set_current_by_name(self, arg0):
          """Select by name
          """
          comm = self.commands.get('\x18\x04')
          return self.z48.execute(comm, (arg0, ))

     def get_current_handle(self):
          """Get current handle

          Returns:
               aksy.sysex.DWORD
          """
          comm = self.commands.get('\x18\x05')
          return self.z48.execute(comm, ())

     def get_current_name(self):
          """Get name of current item

          Returns:
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x18\x06')
          return self.z48.execute(comm, ())

     def get_name_by_handle(self, arg0):
          """Get item name from handle

          Returns:
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x18\x07')
          return self.z48.execute(comm, (arg0, ))

     def get_handle_by_name(self, arg0):
          """Get item handle from name

          Returns:
               aksy.sysex.DWORD
          """
          comm = self.commands.get('\x18\x08')
          return self.z48.execute(comm, (arg0, ))

     def delete_all(self):
          """Delete ALL items from memory
          """
          comm = self.commands.get('\x18\x09')
          return self.z48.execute(comm, ())

     def delete_current(self):
          """Delete current item from memory
          """
          comm = self.commands.get('\x18\x0A')
          return self.z48.execute(comm, ())

     def delete_by_handle(self, arg0):
          """Delete item represented by handle <Data1>
          """
          comm = self.commands.get('\x18\x0B')
          return self.z48.execute(comm, (arg0, ))

     def rename_current(self, arg0):
          """Rename current item
          """
          comm = self.commands.get('\x18\x0C')
          return self.z48.execute(comm, (arg0, ))

     def rename_by_handle(self, arg0, arg1):
          """Rename item represented by handle <Data1>
          """
          comm = self.commands.get('\x18\x0D')
          return self.z48.execute(comm, (arg0, arg1, ))

     def tag(self, arg0, arg1, arg2):
          """Set Tag Bit <Data1> = bit to set, <Data2> = (0=OFF, 1=ON) <Data3> = (0=CURRENT, 1=ALL)
          """
          comm = self.commands.get('\x18\x0E')
          return self.z48.execute(comm, (arg0, arg1, arg2, ))

     def get_tag_bitmap(self):
          """Get Tag Bitmap

          Returns:
               aksy.sysex.WORD
          """
          comm = self.commands.get('\x18\x0F')
          return self.z48.execute(comm, ())

     def get_current_modified(self):
          """Get name of current item with modified/tagged info.

          Returns:
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x18\x10')
          return self.z48.execute(comm, ())

     def get_group_id(self):
          """Get Group ID

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x01')
          return self.z48.execute(comm, ())

     def get_multi_select_method(self):
          """Get Multi Select <Reply1> = (0=OFF, 1=BANK, 2=PROG CHANGE)

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x02')
          return self.z48.execute(comm, ())

     def get_multi_select_channel(self):
          """Get Multi Select Channel <Reply1> = (1A=0, 2A=1, ..., 16B=31)

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x03')
          return self.z48.execute(comm, ())

     def get_multi_tempo(self):
          """Get Multi Tempo <Reply> = 10�bpm

          Returns:
               aksy.sysex.WORD
          """
          comm = self.commands.get('\x1B\x04')
          return self.z48.execute(comm, ())

     def get_multi_program_no(self):
          """Get Multi Program Number

          Returns:
               aksy.sysex.WORD
          """
          comm = self.commands.get('\x1B\x08')
          return self.z48.execute(comm, ())

     def get_multi_part_handle(self, arg0):
          """Get Multi Part handle. <Data1> = Part Number;<Reply> = Handle of program>

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x09')
          return self.z48.execute(comm, (arg0, ))

     def get_multi_part_name(self, arg0):
          """Get Multi Part name. <Data1> = Part Number; <Reply> = Name of part

          Returns:
               aksy.sysex.STRING
          """
          comm = self.commands.get('\x1B\x0A')
          return self.z48.execute(comm, (arg0, ))

     def get_no_parts(self):
          """Get Number of Parts. <Reply> = new number of parts

          Returns:
               aksy.sysex.WORD
          """
          comm = self.commands.get('\x1B\x0F')
          return self.z48.execute(comm, ())

     def get_part_midi_channel(self, arg0):
          """Get Part MIDI Channel, (Data1 = Part Number-1a) <Reply> = (1A=0, 2A=1, ..., 16B=31)

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x10')
          return self.z48.execute(comm, (arg0, ))

     def get_part_mute(self, arg0):
          """Get Part Mute, <Reply> = (0=OFF, 1=ON)

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x11')
          return self.z48.execute(comm, (arg0, ))

     def get_part_solo(self, arg0):
          """Get Part Solo, <Reply> = (0=OFF, 1=ON)

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x12')
          return self.z48.execute(comm, (arg0, ))

     def get_part_level(self):
          """Get Part Level, <Reply> = PartLevel in 10�dB

          Returns:
               aksy.sysex.SWORD
          """
          comm = self.commands.get('\x1B\x13')
          return self.z48.execute(comm, ())

     def get_part_output(self, arg0, arg1):
          """Get Part Output, <Reply> = (Output: 0

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x14')
          return self.z48.execute(comm, (arg0, arg1, ))

     def get_part_pan(self, arg0):
          """Get Part Pan/Balance, <Reply> = Pan/Bal (0�100 = L50�R50); centre=50

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x15')
          return self.z48.execute(comm, (arg0, ))

     def get_part_fx_channel(self, arg0):
          """Get Part Effects Channel: Reply = (0=OFF, 1=FX1, 2=FX2, 3=RV3, 4=RV4)

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x16')
          return self.z48.execute(comm, (arg0, ))

     def get_part_fx_send_level(self):
          """Get Part FX Send Level <Reply> = level in 10�dB

          Returns:
               aksy.sysex.SWORD
          """
          comm = self.commands.get('\x1B\x17')
          return self.z48.execute(comm, ())

     def get_part_tune(self, arg0):
          """Get Part Cents Tune

          Returns:
               aksy.sysex.SWORD
          """
          comm = self.commands.get('\x1B\x18')
          return self.z48.execute(comm, (arg0, ))

     def get_part_low_note(self, arg0):
          """Get Part Low Note

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x1A')
          return self.z48.execute(comm, (arg0, ))

     def get_part_high_note(self, arg0):
          """Get Part High Note

          Returns:
               aksy.sysex.BYTE
          """
          comm = self.commands.get('\x1B\x1B')
          return self.z48.execute(comm, (arg0, ))

     def set_group_id(self, arg0):
          """Set Group ID
          """
          comm = self.commands.get('\x1A\x01')
          return self.z48.execute(comm, (arg0, ))

     def set_multi_select_method(self, arg0):
          """Set Multi Select <Data1> = (0=OFF, 1=BANK, 2=PROG CHANGE)
          """
          comm = self.commands.get('\x1A\x02')
          return self.z48.execute(comm, (arg0, ))

     def set_multi_select_channel(self, arg0):
          """Set Multi Select Channel <Data1> = (1A=0, 2A=1, ..., 16B=31)
          """
          comm = self.commands.get('\x1A\x03')
          return self.z48.execute(comm, (arg0, ))

     def set_multi_tempo(self, arg0):
          """Set Multi Tempo <Data1> = 10�bpm
          """
          comm = self.commands.get('\x1A\x04')
          return self.z48.execute(comm, (arg0, ))

     def set_multi_program_no(self, arg0):
          """Set Multi Program Number Data1: (0=OFF, 1�128)
          """
          comm = self.commands.get('\x1A\x08')
          return self.z48.execute(comm, (arg0, ))

     def set_multi_part_by_handle(self, arg0, arg1):
          """Set Multi Part by handle <Data1> = Part Number; <Data2> = Handle of program
          """
          comm = self.commands.get('\x1A\x09')
          return self.z48.execute(comm, (arg0, arg1, ))
