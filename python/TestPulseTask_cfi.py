testPulseTask = dict(
    MEs = dict(
        Occupancy = dict(path = "Occupancy/TestPulseTask digi occupancy G%(gain)s", otype = 'Ecal2P', btype = 'SuperCrystal', kind = 'TH2F'),
        Shape = dict(path = "TestPulse/Gain%(gain)s/Shape/TestPulseTask shape G%(gain)s", otype = 'SM', btype = 'SuperCrystal', kind = 'TProfile2D', yaxis = {'nbins': 10, 'low': 0., 'high': 10.}),
        Amplitude = dict(path = "TestPulse/Gain%(gain)s/Amplitude/TestPulseTask amplitude G%(gain)s", otype = 'SM', btype = 'Crystal', kind = 'TProfile2D'),
        PNOccupancy = dict(path = "Occupancy/TestPulseTask PN digi occupancy G%(pngain)s", otype = 'MEM', btype = 'Crystal', kind = 'TH2F'),
        PNAmplitude = dict(path = "TestPulse/PN/Gain%(pngain)s/Amplitude/TestPulseTask PN amplitude G%(pngain)s", otype = 'SMMEM', btype = 'Crystal', kind = 'TProfile')
    )
)

