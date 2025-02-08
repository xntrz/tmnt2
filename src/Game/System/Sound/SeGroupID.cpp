#include "SeGroupID.hpp"


struct SEGROUPINFO
{
    GAMETEXT m_TextID;
    bool m_bCallFreeFlag;
    bool m_bHelpFlag;
};


static const SEGROUPINFO s_aSeGroupID[] =
{
    { GAMETEXT(0x0),     false,  false   }, // 0
    { GAMETEXT(0x0),     true,   false   }, // 1
    { GAMETEXT(0x0),     false,  false   }, // 2
    { GAMETEXT(0x0),     true,   false   }, // 3
    { GAMETEXT(0x45E),   false,  true    }, // 4
    { GAMETEXT(0x45F),   false,  true    }, // 5
    { GAMETEXT(0x460),   false,  true    }, // 6
    { GAMETEXT(0x460),   false,  true    }, // 7
    { GAMETEXT(0x461),   false,  true    }, // 8
    { GAMETEXT(0x0),     true,   false   }, // 9
    { GAMETEXT(0x0),     true,   false   }, // 10
    { GAMETEXT(0x0),     true,   false   }, // 11
    { GAMETEXT(0x0),     true,   false   }, // 12
    { GAMETEXT(0x0),     true,   false   }, // 13
    { GAMETEXT(0x0),     true,   false   }, // 14
    { GAMETEXT(0x0),     true,   false   }, // 15
    { GAMETEXT(0x0),     true,   false   }, // 16
    { GAMETEXT(0x0),     true,   false   }, // 17
    { GAMETEXT(0x0),     true,   false   }, // 18
    { GAMETEXT(0x0),     true,   false   }, // 19
    { GAMETEXT(0x0),     true,   false   }, // 20
    { GAMETEXT(0x0),     true,   false   }, // 21
    { GAMETEXT(0x0),     true,   false   }, // 22
    { GAMETEXT(0x462),   true,   true    }, // 23
    { GAMETEXT(0x463),   true,   true    }, // 24
    { GAMETEXT(0x0),     true,   false   }, // 25
    { GAMETEXT(0x0),     true,   false   }, // 26
    { GAMETEXT(0x0),     true,   false   }, // 27
    { GAMETEXT(0x0),     true,   false   }, // 28
    { GAMETEXT(0x0),     true,   false   }, // 29
    { GAMETEXT(0x0),     true,   false   }, // 30
    { GAMETEXT(0x0),     true,   false   }, // 31
    { GAMETEXT(0x0),     true,   false   }, // 32
    { GAMETEXT(0x0),     true,   false   }, // 33
    { GAMETEXT(0x0),     true,   false   }, // 34
    { GAMETEXT(0x0),     true,   false   }, // 35
    { GAMETEXT(0x0),     true,   false   }, // 36
    { GAMETEXT(0x0),     true,   false   }, // 37
    { GAMETEXT(0x0),     true,   false   }, // 38
    { GAMETEXT(0x0),     true,   false   }, // 39
    { GAMETEXT(0x0),     true,   false   }, // 40
    { GAMETEXT(0x0),     true,   false   }, // 41
    { GAMETEXT(0x0),     true,   false   }, // 42
    { GAMETEXT(0x0),     true,   false   }, // 43
    { GAMETEXT(0x0),     true,   false   }, // 44
    { GAMETEXT(0x0),     true,   false   }, // 45
    { GAMETEXT(0x0),     false,  false   }, // 46
    { GAMETEXT(0x0),     false,  false   }, // 47
    { GAMETEXT(0x0),     false,  false   }, // 48
    { GAMETEXT(0x0),     false,  false   }, // 49
    { GAMETEXT(0x0),     false,  false   }, // 50
    { GAMETEXT(0x0),     false,  false   }, // 51
    { GAMETEXT(0x0),     false,  false   }, // 52
    { GAMETEXT(0x0),     false,  false   }, // 53
    { GAMETEXT(0x0),     false,  false   }, // 54
    { GAMETEXT(0x0),     false,  false   }, // 55
    { GAMETEXT(0x0),     false,  false   }, // 56
    { GAMETEXT(0x0),     false,  false   }, // 57
    { GAMETEXT(0x0),     true,   false   }, // 58
    { GAMETEXT(0x0),     false,  false   }, // 59
    { GAMETEXT(0x0),     false,  false   }, // 60
    { GAMETEXT(0x0),     false,  false   }, // 61
    { GAMETEXT(0x0),     false,  false   }, // 62
    { GAMETEXT(0x0),     true,   false   }, // 63
    { GAMETEXT(0x0),     false,  true    }, // 64
    { GAMETEXT(0x0),     false,  false   }, // 65
    { GAMETEXT(0x0),     true,   false   }, // 66
    { GAMETEXT(0x0),     false,  false   }, // 67
    { GAMETEXT(0x464),   false,  true    }, // 68
    { GAMETEXT(0x465),   false,  true    }, // 69
    { GAMETEXT(0x0),     false,  false   }, // 70
    { GAMETEXT(0x0),     false,  false   }, // 71
    { GAMETEXT(0x0),     false,  false   }, // 72
    { GAMETEXT(0x466),   false,  true    }, // 73
    { GAMETEXT(0x0),     true,   false   }, // 74
    { GAMETEXT(0x0),     false,  false   }, // 75
    { GAMETEXT(0x0),     false,  false   }, // 76
    { GAMETEXT(0x467),   false,  true    }, // 77
    { GAMETEXT(0x468),   false,  false   }, // 78
    { GAMETEXT(0x0),     false,  false   }, // 79
    { GAMETEXT(0x0),     false,  false   }, // 80
    { GAMETEXT(0x0),     false,  false   }, // 81
    { GAMETEXT(0x0),     false,  false   }, // 82
    { GAMETEXT(0x0),     false,  false   }, // 83
    { GAMETEXT(0x0),     true,   false   }, // 84
    { GAMETEXT(0x0),     false,  false   }, // 85
    { GAMETEXT(0x469),   false,  true    }, // 86
    { GAMETEXT(0x0),     false,  false   }, // 87
    { GAMETEXT(0x0),     false,  false   }, // 88
    { GAMETEXT(0x0),     true,   false   }, // 89
    { GAMETEXT(0x0),     false,  false   }, // 90
    { GAMETEXT(0x0),     false,  false   }, // 91
    { GAMETEXT(0x46A),   false,  true    }, // 92
    { GAMETEXT(0x0),     false,  false   }, // 93
    { GAMETEXT(0x0),     false,  false   }, // 94
    { GAMETEXT(0x46A),   true,   true    }, // 95
    { GAMETEXT(0x0),     true,   false   }, // 96
    { GAMETEXT(0x0),     false,  false   }, // 97
    { GAMETEXT(0x0),     false,  false   }, // 98
    { GAMETEXT(0x0),     false,  false   }, // 99
    { GAMETEXT(0x0),     false,  false   }, // 100
    { GAMETEXT(0x46B),   false,  false   }, // 101
    { GAMETEXT(0x0),     false,  false   }, // 102
    { GAMETEXT(0x0),     false,  false   }, // 103
    { GAMETEXT(0x0),     false,  false   }, // 104
    { GAMETEXT(0x0),     false,  false   }, // 105
    { GAMETEXT(0x0),     false,  false   }, // 106
    { GAMETEXT(0x0),     false,  false   }, // 107
    { GAMETEXT(0x0),     false,  false   }, // 108
    { GAMETEXT(0x0),     false,  false   }, // 109
    { GAMETEXT(0x0),     false,  false   }, // 110
    { GAMETEXT(0x0),     false,  false   }, // 111
    { GAMETEXT(0x0),     true,   false   }, // 112
    { GAMETEXT(0x0),     false,  false   }, // 113
    { GAMETEXT(0x0),     false,  false   }, // 114
    { GAMETEXT(0x0),     false,  false   }, // 115
    { GAMETEXT(0x0),     true,   false   }, // 116
    { GAMETEXT(0x0),     false,  false   }, // 117
    { GAMETEXT(0x0),     false,  false   }, // 118
    { GAMETEXT(0x0),     true,   false   }, // 119
    { GAMETEXT(0x0),     true,   false   }, // 120
    { GAMETEXT(0x0),     false,  false   }, // 121
    { GAMETEXT(0x0),     false,  false   }, // 122
    { GAMETEXT(0x0),     false,  false   }, // 123
    { GAMETEXT(0x0),     false,  false   }, // 124
    { GAMETEXT(0x0),     false,  false   }, // 125
    { GAMETEXT(0x0),     false,  false   }, // 126
    { GAMETEXT(0x0),     false,  false   }, // 127
    { GAMETEXT(0x0),     false,  false   }, // 128
    { GAMETEXT(0x0),     false,  false   }, // 129
    { GAMETEXT(0x0),     false,  false   }, // 130
    { GAMETEXT(0x0),     false,  false   }, // 131
    { GAMETEXT(0x0),     false,  false   }, // 132
    { GAMETEXT(0x0),     false,  false   }, // 133
    { GAMETEXT(0x0),     true,   false   }, // 134
    { GAMETEXT(0x0),     false,  false   }, // 135
    { GAMETEXT(0x0),     false,  false   }, // 136
    { GAMETEXT(0x0),     false,  false   }, // 137
    { GAMETEXT(0x0),     true,   false   }, // 138
    { GAMETEXT(0x0),     false,  false   }, // 139
    { GAMETEXT(0x0),     false,  false   }, // 140
    { GAMETEXT(0x0),     false,  false   }, // 141
    { GAMETEXT(0x0),     false,  false   }, // 142
    { GAMETEXT(0x0),     false,  false   }, // 143
    { GAMETEXT(0x0),     false,  false   }, // 144
    { GAMETEXT(0x0),     false,  false   }, // 145
    { GAMETEXT(0x0),     false,  false   }, // 146
    { GAMETEXT(0x0),     false,  false   }, // 147
    { GAMETEXT(0x0),     false,  false   }, // 148
    { GAMETEXT(0x0),     false,  false   }, // 149
    { GAMETEXT(0x0),     false,  false   }, // 150
    { GAMETEXT(0x46C),   true,   true    }, // 151
    { GAMETEXT(0x0),     true,   false   }, // 152
    { GAMETEXT(0x0),     false,  false   }, // 153
    { GAMETEXT(0x0),     true,   false   }, // 154
    { GAMETEXT(0x0),     true,   false   }, // 155
    { GAMETEXT(0x0),     true,   false   }, // 156
    { GAMETEXT(0x0),     true,   false   }, // 157
    { GAMETEXT(0x0),     false,  false   }, // 158
    { GAMETEXT(0x0),     true,   false   }, // 159
    { GAMETEXT(0x0),     true,   false   }, // 160
    { GAMETEXT(0x0),     true,   false   }, // 161
    { GAMETEXT(0x0),     true,   false   }, // 162
    { GAMETEXT(0x46D),   true,   false   }, // 163
    { GAMETEXT(0x0),     true,   false   }, // 164
    { GAMETEXT(0x0),     true,   false   }, // 165
    { GAMETEXT(0x0),     true,   false   }, // 166
    { GAMETEXT(0x0),     false,  false   }, // 167
    { GAMETEXT(0x0),     false,  false   }, // 168
    { GAMETEXT(0x0),     false,  false   }, // 169
    { GAMETEXT(0x0),     false,  false   }, // 170
    { GAMETEXT(0x0),     false,  false   }, // 171
    { GAMETEXT(0x0),     false,  false   }, // 172
    { GAMETEXT(0x0),     false,  false   }, // 173
    { GAMETEXT(0x0),     false,  false   }, // 174
    { GAMETEXT(0x0),     false,  false   }, // 175
    { GAMETEXT(0x0),     false,  false   }, // 176
    { GAMETEXT(0x0),     false,  false   }, // 177
    { GAMETEXT(0x0),     false,  false   }, // 178
    { GAMETEXT(0x0),     false,  false   }, // 179
    { GAMETEXT(0x0),     false,  false   }, // 180
    { GAMETEXT(0x0),     false,  false   }, // 181
    { GAMETEXT(0x0),     false,  false   }, // 182
    { GAMETEXT(0x0),     false,  false   }, // 183
    { GAMETEXT(0x0),     false,  false   }, // 184
    { GAMETEXT(0x0),     false,  false   }, // 185
    { GAMETEXT(0x0),     false,  false   }, // 186
    { GAMETEXT(0x0),     false,  false   }, // 187
    { GAMETEXT(0x0),     false,  false   }, // 188
    { GAMETEXT(0x0),     false,  false   }, // 189
    { GAMETEXT(0x0),     false,  false   }, // 190
    { GAMETEXT(0x0),     false,  false   }, // 191
    { GAMETEXT(0x0),     false,  false   }, // 192
    { GAMETEXT(0x0),     false,  false   }, // 193
    { GAMETEXT(0x46E),   false,  true    }, // 194
    { GAMETEXT(0x0),     false,  false   }, // 195
    { GAMETEXT(0x0),     false,  false   }, // 196
    { GAMETEXT(0x0),     true,   false   }, // 197
    { GAMETEXT(0x0),     true,   false   }, // 198
    { GAMETEXT(0x0),     false,  false   }, // 199
    { GAMETEXT(0x0),     false,  false   }, // 200
    { GAMETEXT(0x46F),   false,   true   }, // 201
    { GAMETEXT(0x0),     false,  false   }, // 202
    { GAMETEXT(0x0),     false,  false   }, // 203
    { GAMETEXT(0x0),     false,  false   }, // 204
    { GAMETEXT(0x0),     true,   false   }, // 205
    { GAMETEXT(0x470),   false,   true   }, // 206
    { GAMETEXT(0x0),     false,  false   }, // 207
    { GAMETEXT(0x0),     false,  false   }, // 208
    { GAMETEXT(0x0),     false,  false   }, // 209
    { GAMETEXT(0x0),     false,  false   }, // 210
    { GAMETEXT(0x471),   false,   true   }, // 211
    { GAMETEXT(0x0),     true,   false   }, // 212
    { GAMETEXT(0x0),     true,   false   }, // 213
    { GAMETEXT(0x0),     true,   false   }, // 214
    { GAMETEXT(0x472),   false,  false   }, // 215
    { GAMETEXT(0x473),   false,  false   }, // 216
    { GAMETEXT(0x0),     false,  false   }, // 217
    { GAMETEXT(0x0),     false,  false   }, // 218
    { GAMETEXT(0x460),   false,   true   }, // 219
    { GAMETEXT(0x0),     true,   false   }, // 220
    { GAMETEXT(0x474),   false,   true   }, // 221
    { GAMETEXT(0x474),   false,   true   }, // 222
    { GAMETEXT(0x475),   false,   true   }, // 223
    { GAMETEXT(0x0),     false,  false   }, // 224
    { GAMETEXT(0x0),     false,  false   }, // 225
    { GAMETEXT(0x474),   false,   true   }, // 226
    { GAMETEXT(0x476),   false,  false   }, // 227
    { GAMETEXT(0x477),   false,  false   }, // 228
    { GAMETEXT(0x478),   false,  false   }, // 229
    { GAMETEXT(0x479),   false,  false   }, // 230
    { GAMETEXT(0x47A),   false,  false   }, // 231
};


static inline const SEGROUPINFO& GetSeGroupInfo(SEGROUPID::VALUE id)
{
    ASSERT(id >= 0 && id < COUNT_OF(s_aSeGroupID));
    return s_aSeGroupID[id];
};


namespace SEGROUPID
{
    bool IsHelpID(VALUE id)
    {
        return GetSeGroupInfo(id).m_bHelpFlag;
    };
    

    bool IsCallFree(VALUE id)
    {
        return GetSeGroupInfo(id).m_bCallFreeFlag;
    };


    GAMETEXT GetGameText(VALUE id)
    {
        return GetSeGroupInfo(id).m_TextID;
    };
};