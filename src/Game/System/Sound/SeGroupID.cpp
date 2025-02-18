#include "SeGroupID.hpp"


struct SEGROUPINFO
{
    GAMETEXT textId;
    bool     bCallFreeFlag;
    bool     bHelpFlag;
};


static const SEGROUPINFO s_aSeGroupInfo[] =
{
    {  GAMETEXT_EMPTY,              false,  false   }, // 0
    {  GAMETEXT_EMPTY,              true,   false   }, // 1
    {  GAMETEXT_EMPTY,              false,  false   }, // 2
    {  GAMETEXT_EMPTY,              true,   false   }, // 3
    {  GAMETEXT_HELP_SEMSG_1,       false,  true    }, // 4
    {  GAMETEXT_HELP_SEMSG_2,       false,  true    }, // 5
    {  GAMETEXT_HELP_SEMSG_3,       false,  true    }, // 6
    {  GAMETEXT_HELP_SEMSG_3,       false,  true    }, // 7
    {  GAMETEXT_HELP_SEMSG_4,       false,  true    }, // 8
    {  GAMETEXT_EMPTY,              true,   false   }, // 9
    {  GAMETEXT_EMPTY,              true,   false   }, // 10
    {  GAMETEXT_EMPTY,              true,   false   }, // 11
    {  GAMETEXT_EMPTY,              true,   false   }, // 12
    {  GAMETEXT_EMPTY,              true,   false   }, // 13
    {  GAMETEXT_EMPTY,              true,   false   }, // 14
    {  GAMETEXT_EMPTY,              true,   false   }, // 15
    {  GAMETEXT_EMPTY,              true,   false   }, // 16
    {  GAMETEXT_EMPTY,              true,   false   }, // 17
    {  GAMETEXT_EMPTY,              true,   false   }, // 18
    {  GAMETEXT_EMPTY,              true,   false   }, // 19
    {  GAMETEXT_EMPTY,              true,   false   }, // 20
    {  GAMETEXT_EMPTY,              true,   false   }, // 21
    {  GAMETEXT_EMPTY,              true,   false   }, // 22
    {  GAMETEXT_HELP_SEMSG_5,       true,   true    }, // 23
    {  GAMETEXT_HELP_SEMSG_6,       true,   true    }, // 24
    {  GAMETEXT_EMPTY,              true,   false   }, // 25
    {  GAMETEXT_EMPTY,              true,   false   }, // 26
    {  GAMETEXT_EMPTY,              true,   false   }, // 27
    {  GAMETEXT_EMPTY,              true,   false   }, // 28
    {  GAMETEXT_EMPTY,              true,   false   }, // 29
    {  GAMETEXT_EMPTY,              true,   false   }, // 30
    {  GAMETEXT_EMPTY,              true,   false   }, // 31
    {  GAMETEXT_EMPTY,              true,   false   }, // 32
    {  GAMETEXT_EMPTY,              true,   false   }, // 33
    {  GAMETEXT_EMPTY,              true,   false   }, // 34
    {  GAMETEXT_EMPTY,              true,   false   }, // 35
    {  GAMETEXT_EMPTY,              true,   false   }, // 36
    {  GAMETEXT_EMPTY,              true,   false   }, // 37
    {  GAMETEXT_EMPTY,              true,   false   }, // 38
    {  GAMETEXT_EMPTY,              true,   false   }, // 39
    {  GAMETEXT_EMPTY,              true,   false   }, // 40
    {  GAMETEXT_EMPTY,              true,   false   }, // 41
    {  GAMETEXT_EMPTY,              true,   false   }, // 42
    {  GAMETEXT_EMPTY,              true,   false   }, // 43
    {  GAMETEXT_EMPTY,              true,   false   }, // 44
    {  GAMETEXT_EMPTY,              true,   false   }, // 45
    {  GAMETEXT_EMPTY,              false,  false   }, // 46
    {  GAMETEXT_EMPTY,              false,  false   }, // 47
    {  GAMETEXT_EMPTY,              false,  false   }, // 48
    {  GAMETEXT_EMPTY,              false,  false   }, // 49
    {  GAMETEXT_EMPTY,              false,  false   }, // 50
    {  GAMETEXT_EMPTY,              false,  false   }, // 51
    {  GAMETEXT_EMPTY,              false,  false   }, // 52
    {  GAMETEXT_EMPTY,              false,  false   }, // 53
    {  GAMETEXT_EMPTY,              false,  false   }, // 54
    {  GAMETEXT_EMPTY,              false,  false   }, // 55
    {  GAMETEXT_EMPTY,              false,  false   }, // 56
    {  GAMETEXT_EMPTY,              false,  false   }, // 57
    {  GAMETEXT_EMPTY,              true,   false   }, // 58
    {  GAMETEXT_EMPTY,              false,  false   }, // 59
    {  GAMETEXT_EMPTY,              false,  false   }, // 60
    {  GAMETEXT_EMPTY,              false,  false   }, // 61
    {  GAMETEXT_EMPTY,              false,  false   }, // 62
    {  GAMETEXT_EMPTY,              true,   false   }, // 63
    {  GAMETEXT_EMPTY,              false,  true    }, // 64
    {  GAMETEXT_EMPTY,              false,  false   }, // 65
    {  GAMETEXT_EMPTY,              true,   false   }, // 66
    {  GAMETEXT_EMPTY,              false,  false   }, // 67
    {  GAMETEXT_HELP_SEMSG_7,       false,  true    }, // 68
    {  GAMETEXT_HELP_SEMSG_8,       false,  true    }, // 69
    {  GAMETEXT_EMPTY,              false,  false   }, // 70
    {  GAMETEXT_EMPTY,              false,  false   }, // 71
    {  GAMETEXT_EMPTY,              false,  false   }, // 72
    {  GAMETEXT_HELP_SEMSG_9,       false,  true    }, // 73
    {  GAMETEXT_EMPTY,              true,   false   }, // 74
    {  GAMETEXT_EMPTY,              false,  false   }, // 75
    {  GAMETEXT_EMPTY,              false,  false   }, // 76
    {  GAMETEXT_HELP_SEMSG_10,      false,  true    }, // 77
    {  GAMETEXT_HELP_SEMSG_11,      false,  false   }, // 78
    {  GAMETEXT_EMPTY,              false,  false   }, // 79
    {  GAMETEXT_EMPTY,              false,  false   }, // 80
    {  GAMETEXT_EMPTY,              false,  false   }, // 81
    {  GAMETEXT_EMPTY,              false,  false   }, // 82
    {  GAMETEXT_EMPTY,              false,  false   }, // 83
    {  GAMETEXT_EMPTY,              true,   false   }, // 84
    {  GAMETEXT_EMPTY,              false,  false   }, // 85
    {  GAMETEXT_HELP_SEMSG_12,      false,  true    }, // 86
    {  GAMETEXT_EMPTY,              false,  false   }, // 87
    {  GAMETEXT_EMPTY,              false,  false   }, // 88
    {  GAMETEXT_EMPTY,              true,   false   }, // 89
    {  GAMETEXT_EMPTY,              false,  false   }, // 90
    {  GAMETEXT_EMPTY,              false,  false   }, // 91
    {  GAMETEXT_HELP_SEMSG_13,      false,  true    }, // 92
    {  GAMETEXT_EMPTY,              false,  false   }, // 93
    {  GAMETEXT_EMPTY,              false,  false   }, // 94
    {  GAMETEXT_HELP_SEMSG_13,      true,   true    }, // 95
    {  GAMETEXT_EMPTY,              true,   false   }, // 96
    {  GAMETEXT_EMPTY,              false,  false   }, // 97
    {  GAMETEXT_EMPTY,              false,  false   }, // 98
    {  GAMETEXT_EMPTY,              false,  false   }, // 99
    {  GAMETEXT_EMPTY,              false,  false   }, // 100
    {  GAMETEXT_HELP_SEMSG_14,      false,  false   }, // 101
    {  GAMETEXT_EMPTY,              false,  false   }, // 102
    {  GAMETEXT_EMPTY,              false,  false   }, // 103
    {  GAMETEXT_EMPTY,              false,  false   }, // 104
    {  GAMETEXT_EMPTY,              false,  false   }, // 105
    {  GAMETEXT_EMPTY,              false,  false   }, // 106
    {  GAMETEXT_EMPTY,              false,  false   }, // 107
    {  GAMETEXT_EMPTY,              false,  false   }, // 108
    {  GAMETEXT_EMPTY,              false,  false   }, // 109
    {  GAMETEXT_EMPTY,              false,  false   }, // 110
    {  GAMETEXT_EMPTY,              false,  false   }, // 111
    {  GAMETEXT_EMPTY,              true,   false   }, // 112
    {  GAMETEXT_EMPTY,              false,  false   }, // 113
    {  GAMETEXT_EMPTY,              false,  false   }, // 114
    {  GAMETEXT_EMPTY,              false,  false   }, // 115
    {  GAMETEXT_EMPTY,              true,   false   }, // 116
    {  GAMETEXT_EMPTY,              false,  false   }, // 117
    {  GAMETEXT_EMPTY,              false,  false   }, // 118
    {  GAMETEXT_EMPTY,              true,   false   }, // 119
    {  GAMETEXT_EMPTY,              true,   false   }, // 120
    {  GAMETEXT_EMPTY,              false,  false   }, // 121
    {  GAMETEXT_EMPTY,              false,  false   }, // 122
    {  GAMETEXT_EMPTY,              false,  false   }, // 123
    {  GAMETEXT_EMPTY,              false,  false   }, // 124
    {  GAMETEXT_EMPTY,              false,  false   }, // 125
    {  GAMETEXT_EMPTY,              false,  false   }, // 126
    {  GAMETEXT_EMPTY,              false,  false   }, // 127
    {  GAMETEXT_EMPTY,              false,  false   }, // 128
    {  GAMETEXT_EMPTY,              false,  false   }, // 129
    {  GAMETEXT_EMPTY,              false,  false   }, // 130
    {  GAMETEXT_EMPTY,              false,  false   }, // 131
    {  GAMETEXT_EMPTY,              false,  false   }, // 132
    {  GAMETEXT_EMPTY,              false,  false   }, // 133
    {  GAMETEXT_EMPTY,              true,   false   }, // 134
    {  GAMETEXT_EMPTY,              false,  false   }, // 135
    {  GAMETEXT_EMPTY,              false,  false   }, // 136
    {  GAMETEXT_EMPTY,              false,  false   }, // 137
    {  GAMETEXT_EMPTY,              true,   false   }, // 138
    {  GAMETEXT_EMPTY,              false,  false   }, // 139
    {  GAMETEXT_EMPTY,              false,  false   }, // 140
    {  GAMETEXT_EMPTY,              false,  false   }, // 141
    {  GAMETEXT_EMPTY,              false,  false   }, // 142
    {  GAMETEXT_EMPTY,              false,  false   }, // 143
    {  GAMETEXT_EMPTY,              false,  false   }, // 144
    {  GAMETEXT_EMPTY,              false,  false   }, // 145
    {  GAMETEXT_EMPTY,              false,  false   }, // 146
    {  GAMETEXT_EMPTY,              false,  false   }, // 147
    {  GAMETEXT_EMPTY,              false,  false   }, // 148
    {  GAMETEXT_EMPTY,              false,  false   }, // 149
    {  GAMETEXT_EMPTY,              false,  false   }, // 150
    {  GAMETEXT_HELP_SEMSG_15,      true,   true    }, // 151
    {  GAMETEXT_EMPTY,              true,   false   }, // 152
    {  GAMETEXT_EMPTY,              false,  false   }, // 153
    {  GAMETEXT_EMPTY,              true,   false   }, // 154
    {  GAMETEXT_EMPTY,              true,   false   }, // 155
    {  GAMETEXT_EMPTY,              true,   false   }, // 156
    {  GAMETEXT_EMPTY,              true,   false   }, // 157
    {  GAMETEXT_EMPTY,              false,  false   }, // 158
    {  GAMETEXT_EMPTY,              true,   false   }, // 159
    {  GAMETEXT_EMPTY,              true,   false   }, // 160
    {  GAMETEXT_EMPTY,              true,   false   }, // 161
    {  GAMETEXT_EMPTY,              true,   false   }, // 162
    {  GAMETEXT_HELP_SEMSG_16,      true,   false   }, // 163
    {  GAMETEXT_EMPTY,              true,   false   }, // 164
    {  GAMETEXT_EMPTY,              true,   false   }, // 165
    {  GAMETEXT_EMPTY,              true,   false   }, // 166
    {  GAMETEXT_EMPTY,              false,  false   }, // 167
    {  GAMETEXT_EMPTY,              false,  false   }, // 168
    {  GAMETEXT_EMPTY,              false,  false   }, // 169
    {  GAMETEXT_EMPTY,              false,  false   }, // 170
    {  GAMETEXT_EMPTY,              false,  false   }, // 171
    {  GAMETEXT_EMPTY,              false,  false   }, // 172
    {  GAMETEXT_EMPTY,              false,  false   }, // 173
    {  GAMETEXT_EMPTY,              false,  false   }, // 174
    {  GAMETEXT_EMPTY,              false,  false   }, // 175
    {  GAMETEXT_EMPTY,              false,  false   }, // 176
    {  GAMETEXT_EMPTY,              false,  false   }, // 177
    {  GAMETEXT_EMPTY,              false,  false   }, // 178
    {  GAMETEXT_EMPTY,              false,  false   }, // 179
    {  GAMETEXT_EMPTY,              false,  false   }, // 180
    {  GAMETEXT_EMPTY,              false,  false   }, // 181
    {  GAMETEXT_EMPTY,              false,  false   }, // 182
    {  GAMETEXT_EMPTY,              false,  false   }, // 183
    {  GAMETEXT_EMPTY,              false,  false   }, // 184
    {  GAMETEXT_EMPTY,              false,  false   }, // 185
    {  GAMETEXT_EMPTY,              false,  false   }, // 186
    {  GAMETEXT_EMPTY,              false,  false   }, // 187
    {  GAMETEXT_EMPTY,              false,  false   }, // 188
    {  GAMETEXT_EMPTY,              false,  false   }, // 189
    {  GAMETEXT_EMPTY,              false,  false   }, // 190
    {  GAMETEXT_EMPTY,              false,  false   }, // 191
    {  GAMETEXT_EMPTY,              false,  false   }, // 192
    {  GAMETEXT_EMPTY,              false,  false   }, // 193
    {  GAMETEXT_HELP_SEMSG_17,      false,  true    }, // 194
    {  GAMETEXT_EMPTY,              false,  false   }, // 195
    {  GAMETEXT_EMPTY,              false,  false   }, // 196
    {  GAMETEXT_EMPTY,              true,   false   }, // 197
    {  GAMETEXT_EMPTY,              true,   false   }, // 198
    {  GAMETEXT_EMPTY,              false,  false   }, // 199
    {  GAMETEXT_EMPTY,              false,  false   }, // 200
    {  GAMETEXT_HELP_SEMSG_18,      false,   true   }, // 201
    {  GAMETEXT_EMPTY,              false,  false   }, // 202
    {  GAMETEXT_EMPTY,              false,  false   }, // 203
    {  GAMETEXT_EMPTY,              false,  false   }, // 204
    {  GAMETEXT_EMPTY,              true,   false   }, // 205
    {  GAMETEXT_HELP_SEMSG_19,      false,   true   }, // 206
    {  GAMETEXT_EMPTY,              false,  false   }, // 207
    {  GAMETEXT_EMPTY,              false,  false   }, // 208
    {  GAMETEXT_EMPTY,              false,  false   }, // 209
    {  GAMETEXT_EMPTY,              false,  false   }, // 210
    {  GAMETEXT_HELP_SEMSG_20,      false,   true   }, // 211
    {  GAMETEXT_EMPTY,              true,   false   }, // 212
    {  GAMETEXT_EMPTY,              true,   false   }, // 213
    {  GAMETEXT_EMPTY,              true,   false   }, // 214
    {  GAMETEXT_HELP_SEMSG_21,      false,  false   }, // 215
    {  GAMETEXT_HELP_SEMSG_22,      false,  false   }, // 216
    {  GAMETEXT_EMPTY,              false,  false   }, // 217
    {  GAMETEXT_EMPTY,              false,  false   }, // 218
    {  GAMETEXT_HELP_SEMSG_3,       false,   true   }, // 219
    {  GAMETEXT_EMPTY,              true,   false   }, // 220
    {  GAMETEXT_HELP_SEMSG_23,      false,   true   }, // 221
    {  GAMETEXT_HELP_SEMSG_23,      false,   true   }, // 222
    {  GAMETEXT_HELP_SEMSG_24,      false,   true   }, // 223
    {  GAMETEXT_EMPTY,              false,  false   }, // 224
    {  GAMETEXT_EMPTY,              false,  false   }, // 225
    {  GAMETEXT_HELP_SEMSG_23,      false,   true   }, // 226
    {  GAMETEXT_HELP_SEMSG_25,      false,  false   }, // 227
    {  GAMETEXT_HELP_SEMSG_26,      false,  false   }, // 228
    {  GAMETEXT_HELP_SEMSG_27,      false,  false   }, // 229
    {  GAMETEXT_HELP_SEMSG_28,      false,  false   }, // 230
    {  GAMETEXT_HELP_SEMSG_29,      false,  false   }, // 231
};


static inline const SEGROUPINFO& SeGroupInfo(SEGROUPID::VALUE id)
{
    ASSERT(id >= 0);
    ASSERT(id < COUNT_OF(s_aSeGroupInfo));

    return s_aSeGroupInfo[id];
};


namespace SEGROUPID
{
    bool IsHelpID(VALUE id)
    {
        return SeGroupInfo(id).bHelpFlag;
    };
    

    bool IsCallFree(VALUE id)
    {
        return SeGroupInfo(id).bCallFreeFlag;
    };


    GAMETEXT GetGameText(VALUE id)
    {
        return SeGroupInfo(id).textId;
    };
};