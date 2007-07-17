import gobject,gtk,aksy,ak

class Envelope(ak.SamplerObject):
    def __init__(self,keygroup,index,xoffset=10,yoffset=10):
        ak.SamplerObject.__init__(self, keygroup.s, keygroup, "keygrouptools", index)
        self.keygroup_index = keygroup.index
        self.need_index_for_set = True
        self.need_index_in_arguments = True
        self.attrs = ["envelope_rate1", "envelope_level1", "envelope_rate2", "envelope_level2",  "envelope_rate3", "envelope_level3", "envelope_rate4", "envelope_level4", "envelope_reference"]
        self.xoffset = xoffset
        self.yoffset = yoffset 
        self.set_envelope(keygroup, index)
        self.precache()

    def updateNode(self, nodeIndex, rate, level):
        # stop it
        level = max(0,min(100,level))
        rate = max(0,min(100,rate))

        rateattr = "envelope_rate" + str(nodeIndex + 1) 
        levelattr = "envelope_level" + str(nodeIndex + 1)

        self.set(rateattr, rate) 
        self.set(levelattr, level) 

    def set_envelope(self, keygroup, index):
        if type(self.index) != int:
            raise Exception("Not an int.")
        self.keygroup = keygroup
        self.index = index 