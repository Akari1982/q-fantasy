#include "browser_field.h"
#include "browser_field_opcodes.h"
#include "system/icons_font.h"
#include "kernel/file.h"
#include "field/field.h"

#include "ofxImGui.h"
#include <format>

bool g_browser_field = false;

std::vector<u8> l_dat_file;

void ParseFieldEvents(u8 actor_id);



void BrowserField()
{
    if (g_browser_field == false) return;

    static bool loaded = false;

    ImGui::SetNextWindowPos(ImGui::GetWindowPos() + ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(900, 650), ImGuiCond_Once);
    if (ImGui::Begin("Field Browser", &g_browser_field, ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Columns(2, nullptr, true);
        // do not reset column width every frame
        static bool first_frame = true;
        if (first_frame)
        {
            ImGui::SetColumnWidth(0, 250);
            first_frame = false;
        }

        static int field_id = 0;

        if (ImGui::BeginListBox("##FieldList", ImVec2(-FLT_MIN, -FLT_MIN)))
        {
            ImGui::TextDisabled("Fields Map List");
            ImGui::Separator();

            for (int i = 0; i < 0x312; ++i)
            {
                ImGui::PushID(i);

                std::string name = g_field_files[i];
                bool corrupted = false;

                if ((name == "BLACKBGA")
                 || (name == "BLACKBGF")
                 || (name == "BLACKBGG")
                 || (name == "BLIN69_2")
                 || (name == "CONVIL_3")
                 || (name == "FALLP")
                 || (name == "FSHIP_26")
                 || (name == "HYOU14")
                 || (name == "JUNMON")
                 || (name == "M_ENDO")
                 || (name == "ONNA_1")
                 || (name == "ONNA_3")
                 || (name == "ONNA_6")
                 || (name == "PASS")
                 || (name == "Q_5")
                 || (name == "SUBIN_4")
                 || (name == "TRAP")
                 || (name == "WHITEBG1")
                 || (name == "WHITEBG2")
                 || (name.compare(0, 2, "WM") == 0)
                 || (name == "XMVTES"))
                {
                    corrupted = true;
                    ImGui::BeginDisabled(true);
                }

                if (ImGui::Selectable(("0x" + std::format("{:03x} ", i) + g_field_files[i] + ((corrupted) ? " (corrupted)": "")).c_str(), (i == field_id)))
                {
                    loaded = false;
                    field_id = i;
                }

                if (corrupted) ImGui::EndDisabled();

                ImGui::PopID();
            }
        }
        ImGui::EndListBox();

        ImGui::NextColumn();
        ImGui::BeginChild("right_panel", ImVec2(0, 0), true);

        if (loaded == false)
        {
            FileLZS("FIELD/" + g_field_files[field_id] + ".DAT", l_dat_file);
            loaded = true;
        }

        ImGui::Text("Map: 0x%03x", field_id);

        u32 ate_addr = READ_LE_U32(&l_dat_file[0x0]) - g_field_dat_base_addr;
        u8 actors_n = READ_LE_U8(&l_dat_file[ate_addr + 0x2]);
        u8 models_n = READ_LE_U8(&l_dat_file[ate_addr + 0x3]);
        u16 strings_ofs = READ_LE_U16(&l_dat_file[ate_addr + 0x4]);
        u16 scale = READ_LE_U16(&l_dat_file[ate_addr + 0x8]);

        ImGui::Text("Event ver%d. Data ver%d", READ_LE_U8(&l_dat_file[ate_addr + 0x0]), READ_LE_U8(&l_dat_file[ate_addr + 0x1]));
        ImGui::Text("Entities: %d. Models: %d", actors_n, models_n);
        ImGui::Text("Scale: 0x%04x", scale);
        ImGui::Text("Authr: %s", &l_dat_file[ate_addr + 0x10]);
        ImGui::Text("Event: %s", &l_dat_file[ate_addr + 0x18]);
        ImGui::Separator();

        for (int i = 0; i < actors_n; ++i)
        {
            if (ImGui::TreeNode(("Actor " + std::format("0x{:02x} ", i) + std::string((char*)&l_dat_file[ate_addr + 0x20 + i * 0x8])).c_str()))
            {
                ParseFieldEvents(i);

                ImGui::TreePop();
            }
        }
        ImGui::Separator();

        ImGui::EndChild();

        ImGui::Columns(1);
    }
    ImGui::End();
}



void ArgGetU8(u32 arg_ofs)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.5, 0, 1));
    ImGui::Text("0x%02x", READ_LE_U8(&l_dat_file[arg_ofs]));
    ImGui::PopStyleColor();
}



void ArgGetJump8(u32 arg_ofs)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 1, 1));
    ImGui::Text("0x%02x", READ_LE_U8(&l_dat_file[arg_ofs]));
    ImGui::PopStyleColor();
}



void ArgGetMemory8(u8 mem_block, u32 op_ofs, u32 arg_ofs)
{
    u8 bank = 0;
    switch (mem_block)
    {
        case 0x1: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x1]) >> 0x4; break;
        case 0x2: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x1]) & 0xf; break;
        case 0x3: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x2]) >> 0x4; break;
        case 0x4: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x2]) & 0xf; break;
        case 0x5: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x3]) >> 0x4; break;
        case 0x6: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x3]) & 0xf; break;
    }

    if (bank == 0x0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.5, 0, 1));
        ImGui::Text("0x%02x", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x1) || (bank == 0x2))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M0_1[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x3) || (bank == 0x4))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M1_1[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0xb) || (bank == 0xc))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M2_1[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0xd) || (bank == 0xe))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M3_1[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x7) || (bank == 0xf))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M4_1[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x5) || (bank == 0x6))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("L_1[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
}



void ArgGetMemory16(u8 mem_block, u32 op_ofs, u32 arg_ofs)
{
    u8 bank = 0;
    switch (mem_block)
    {
        case 0x1: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x1]) >> 0x4; break;
        case 0x2: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x1]) & 0xf; break;
        case 0x3: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x2]) >> 0x4; break;
        case 0x4: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x2]) & 0xf; break;
        case 0x5: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x3]) >> 0x4; break;
        case 0x6: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x3]) & 0xf; break;
    }

    if (bank == 0x0)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.5, 0, 1));
        ImGui::Text("0x%04x", READ_LE_U16(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x1) || (bank == 0x2))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M0_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x3) || (bank == 0x4))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M1_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0xb) || (bank == 0xc))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M2_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0xd) || (bank == 0xe))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M3_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x7) || (bank == 0xf))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M4_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x5) || (bank == 0x6))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("L_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
}



void ArgSetMemory16(u8 mem_block, u32 op_ofs, u32 arg_ofs)
{
    u8 bank = 0;
    switch (mem_block)
    {
        case 0x1: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x1]) >> 0x4; break;
        case 0x2: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x1]) & 0xf; break;
        case 0x3: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x2]) >> 0x4; break;
        case 0x4: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x2]) & 0xf; break;
        case 0x5: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x3]) >> 0x4; break;
        case 0x6: bank = READ_LE_U8(&l_dat_file[op_ofs + 0x3]) & 0xf; break;
    }

    if ((bank == 0x1) || (bank == 0x2))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M0_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x3) || (bank == 0x3))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M1_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0xb) || (bank == 0xc))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M2_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0xd) || (bank == 0xe))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M3_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x7) || (bank == 0xf))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("M4_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
    else if ((bank == 0x5) || (bank == 0x6))
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
        ImGui::Text("L_2[0x%02x]", READ_LE_U8(&l_dat_file[arg_ofs]));
        ImGui::PopStyleColor();
    }
}



void ParseFieldEvents(u8 actor_id)
{
    u32 ate_addr = READ_LE_U32(&l_dat_file[0x0]) - g_field_dat_base_addr;
    u8 actors_n = READ_LE_U8(&l_dat_file[ate_addr + 0x2]);
    u16 akao_n = READ_LE_S16(&l_dat_file[ate_addr + 0x6]);

    for (int i = 0; i < 0x20; ++i)
    {
        u32 event_ofs = READ_LE_U16(&l_dat_file[ate_addr + 0x20 + actors_n * 0x8 + akao_n * 0x4 + actor_id * 0x40 + i * 0x02]);
        u32 end_ofs = event_ofs;
        u8 opcode = READ_LE_U8(&l_dat_file[ate_addr + event_ofs]);

        if (ImGui::TreeNode(("Event " + std::format("0x{:02x} ", i) + std::format("0x{:04x}", event_ofs)).c_str()))
        {
    //        if (opcode == 0x00 && i != 0) // skip if script contain only RET opcode and this is not init script
    //        {
    //            continue;
    //        }
    //        if (prev_script == script)
    //        {
    //            continue;
    //        }
    //        prev_script = script;

            for (; event_ofs <= end_ofs;)
            {
                u8 opcode = READ_LE_U8(&l_dat_file[ate_addr + event_ofs]);

                ImGui::Text("  0x%04x    ", event_ofs);
                ImGui::SameLine(0, 0);

                if (g_field_opcodes[opcode].init)
                {
                    ImGui::Text("%s(", g_field_opcodes[opcode].full_name.c_str()); ImGui::SameLine(0, 0);

                    for (int arg_id = 0; arg_id < g_field_opcodes[opcode].args.size(); ++arg_id)
                    {
                        const OpcodeArg& argument = g_field_opcodes[opcode].args[arg_id];
                        const u32 arg_ofs = ate_addr + event_ofs + argument.offset;

                        if (arg_id != 0) ImGui::Text(","); ImGui::SameLine(0, 0);

                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1));
                        ImGui::Text(" %s:", argument.name.c_str()); ImGui::SameLine(0, 0);
                        ImGui::PopStyleColor();

                        switch(argument.type)
                        {
                            case OpcodeArg::U8: ArgGetU8(arg_ofs); break;
                            case OpcodeArg::READ_MEMORY8: ArgGetMemory8(argument.mem_block, ate_addr + event_ofs, arg_ofs); break;
                            case OpcodeArg::READ_MEMORY16: ArgGetMemory16(argument.mem_block, ate_addr + event_ofs, arg_ofs); break;
                            case OpcodeArg::WRITE_MEMORY16: ArgSetMemory16(argument.mem_block, ate_addr + event_ofs, arg_ofs); break;

                            case OpcodeArg::JUMP8:
                            {
                                ArgGetJump8(arg_ofs);

                                u32 temp_end = event_ofs + READ_LE_U8(&l_dat_file[arg_ofs]) + g_field_opcodes[opcode].size - 1;
                                end_ofs = (temp_end > end_ofs) ? temp_end : end_ofs;
                            }
                            break;
                        }

                        ImGui::SameLine(0, 0);

                        if ((arg_id + 1) >= g_field_opcodes[opcode].args.size()) ImGui::Text(" "); ImGui::SameLine(0, 0);
                    }
                    ImGui::Text(")");

                    event_ofs += g_field_opcodes[opcode].size;
                    end_ofs = (event_ofs > end_ofs) ? event_ofs : end_ofs;

                    ImGui::SameLine(0, 0);
                    ImGui::Text(" // end:0x%04x", end_ofs);
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
                    ImGui::Text("Failed to decode opcode 0x%02X", opcode);
                    ImGui::PopStyleColor();
                    break;
                }
            }

            ImGui::TreePop();
        }
    }
}
