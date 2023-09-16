from NoteValueConverter import Value2Note

class Note:
    def __init__(self, value:int, volume:int, detune:int):
        self.value = value
        self.volume = volume
        self.detune = detune

    def output(self) -> str:
        return str(Value2Note(self.value)) + ".." + hex(self.volume + 0x100)[-2:] + "E5" + hex(self.detune + 0x100)[-2:]