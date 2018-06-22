/*******************************************************************************
* File Name: MyGraphicLCDIntf_v1_00.v
* Version `$CY_MAJOR_VERSION`.`$CY_MINOR_VERSION`
*
* Description:
* The component provides the interface to a graphic LCD controller and driver 
* device. This file provides a top level model of the GraphicLCDIntf module
* defining the controller and datapath instances and all of the necessary
* interconnect
*
*------------------------------------------------------------------------------
*                 Control and Status Register definitions
*------------------------------------------------------------------------------
*    
*   Status Register Definition
*  +=======+-------+------+------+------+------+------+----------+-------+
*  |  Bit  |   7   |  6   |  5   |  4   |  3   |  2   |     1    |   0   |
*  +=======+-------+------+------+------+------+------+----------+-------+
*  | Desc  |                 unused            |      |data_valid|  full |
*  +=======+-------+------+------+------+------+------+----------+-------+
*
*    full           =>  1 = there is no room in the FIFO    
*                       0 = there is room for at least 1 byte in the FIFO
*
*    data_valid     =>  0 = read operation has not completed
*                       1 = read operation has completed
*
*  LSB Status Register Definition
*  +=======+------+------+------+------+------+------+------+------+
*  |  Bit  |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
*  +=======+------+------+------+------+------+------+------+------+
*  | Desc  |          Lower 8 bits of the input data bus           |      
*  +=======+------+------+------+------+------+------+------+------+
*  
*  Used to store the lower 8 bits of read value during a read transaction
*
*  MSB Status Register Definition
*  +=======+------+------+------+------+------+------+------+------+
*  |  Bit  |  7   |  6   |  5   |  4   |  3   |  2   |  1   |  0   |
*  +=======+------+------+------+------+------+------+------+------+
*  | Desc  |          Upper 8 bits of the input data bus           |      
*  +=======+------+------+------+------+------+------+------+------+
*  
*  Used to store the upper 8 bits of read value during a read transaction
*
********************************************************************************
* Data Path register definitions
********************************************************************************
* GraphicLCDIntf: LsbDp
* DESCRIPTION: LsbDp is used for the following operations:
*       - command fetch/decode, 
*       - provides LSB part of output data bus during write transaction
*       - Read High and Low Pulse Width Counter during read transaction
* REGISTER USAGE:
* F0 => Command FIFO
* F1 => Data FIFO
* D0 => Read Low Pulse Width Counter Period
* D1 => Read High Pulse Width Counter Period
* A0 => Command reg, Low Pulse Counter internal reg
* A1 => Data reg, High Pulse Counter internal reg
*
********************************************************************************
* GraphicLCDIntf: MsbDp
* DESCRIPTION: MsbDp is used in order to provide MSB part of output data bus 
* during write transaction. Only present for 16-bit interface mode.
* REGISTER USAGE:
* F0 => not used
* F1 => Data FIFO
* D0 => not used
* D1 => not used
* A0 => Data reg
* A1 => not used
*
********************************************************************************
* I*O Signals:
********************************************************************************
*    output         d_c        Data / Command signal               
*    output  [7:0]  do_lsb     Lower 8 bits of the output data bus 
*    output  [7:0]  do_msb     Upper 8 bits of the output data bus 
*    output         ncs        Active low chip select              
*    output         nrd        Active low read control signal      
*    output         nwr        Active low write control signal     
*    output         oe         Output enable for the data bus      
*    input          clock      Clock that operates this component  
*    input   [7:0]  di_lsb     Lower 8 bits of the output data bus 
*    input   [7:0]  di_msb     Lower 8 bits of the output data bus             
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/


`include "cypress.v"
`ifdef MyGraphicLCDIntf_V_v1_00_ALREADY_INCLUDED
`else
`define MyGraphicLCDIntf_V_v1_00_ALREADY_INCLUDED

module MyGraphicLCDIntf_v1_00 (
    output   reg         d_c,       /* Data / Command signal               */
    output   wire [7:0]  do_lsb,    /* Lower 8 bits of the output data bus */
    output   wire [7:0]  do_msb,    /* Upper 8 bits of the output data bus */
    output   reg         ncs,       /* Active low chip select              */
    output   reg         nrd,       /* Active low read control signal      */
    output   reg         nwr,       /* Active low write control signal     */
    output   reg         oe,        /* Output enable for the data bus      */
    input    wire        clock,     /* Clock that operates this component  */
    input    wire [7:0]  di_lsb,    /* Lower 8 bits of the output data bus */
    input    wire [7:0]  di_msb     /* Lower 8 bits of the output data bus */
);

    /***************************************************************************
    *               Parameters                                                 *
    ***************************************************************************/
    parameter [7:0] BitWidth    = 8'd16;
    parameter [7:0] ReadHiPulse = 8'd10;
    parameter [7:0] ReadLoPulse = 8'd10;
    
    localparam [7:0] LoReadPulseCntPeriod = ReadLoPulse-2;
    localparam [7:0] HiReadPulseCntPeriod = ReadHiPulse-1;
    
    localparam [7:0] BIT_WIDTH_16 = 8'd16;
    localparam [7:0] BIT_WIDTH_8  = 8'd8;
    
    /***************************************************************************
    *            Combinatorial signals                                         *
    ***************************************************************************/
    
    wire z1_detect;      /* zero detect bit used to count hi read pulse      */
    wire z0_detect;      /* zero detect bit used to count lo read pulse      */
    
    wire data_empty;     /* Data FIFO empty status signal to internal FSM    */
    wire cmd_empty;      /* Command FIFO empty status signal to internal FSM */
    wire cmd_not_full;   /* Command FIFO not full status to system bus       */ 
    wire data_not_full;  /* Data FIFO not full status to system bus          */
    wire data_valid;     /* signal to system that input data is valid        */
    wire [7:0] data_lsb; /* lower part of data bus                           */
    
    wire cmd;            /* signal to command decoding                       */
    wire data_ready;     /* provide data ready status to state machine       */
    wire cmd_ready;      /* provide command ready status to state machine    */
    wire op_clk;         /* internal clock to drive the component            */
    wire din_clk; 
    
    assign cmd = data_lsb[1];
    
    assign cmd_ready  = ~cmd_empty;
    assign data_ready = ~data_empty;
    
    /***************************************************************************
    *         Instantiation of udb_clock_enable primitive 
    ****************************************************************************
    * The udb_clock_enable primitive component allows to support clock enable 
    * mechanism and specify the intended synchronization behavior for the clock 
    * result (operational clock).
    * There is no need to reset or enable this component. In this case the 
    * udb_clock_enable is used only for synchronization. The resulted clock is 
    * always enabled.
    */
    cy_psoc3_udb_clock_enable_v1_0 #(.sync_mode(`TRUE)) ClkSync
    (
        /* input  */    .clock_in(clock),
        /* input  */    .enable(1'b1),
        /* output */    .clock_out(op_clk)
    );  

    /***************************************************************************
    *                   Status registers implementation                        *
    ****************************************************************************
    * Two or three status registers are used depending on data bus bit width.
    * The first Status register is used in order to know that the read has 
    * completed and to keep the FIFO0 (Command and data FIFO) from overflowing 
    * for read and write APIs
    *
    * The rest status registers are used to store read value for the CPU. 
    * Depending on bit width it is required to use one status for 8 bits or two
    * status for 16 bit.
    * 
    ***************************************************************************/
    
    /* Status register bit locations (bit 7-2 unused) */
    localparam CMD_FIFO_FULL   = 3'd0;    /* there is no room in the Cmd FIFO */
    localparam DATA_VALID      = 3'd1;    /* read data is valid */ 
    wire [7:0] status;

    /***************************************************************************
    * FIFO full calculation is based on the command DP and the data DP. The 
    * command DP FIFO status is used since the data DP is not used for reads.  
    * The data DP is used since the data DP is read after the command DP so it
    * can be full even with the command DP is no longer full.
    ***************************************************************************/
    wire full = ~(cmd_not_full & data_not_full);
    
    assign status[CMD_FIFO_FULL]   = full;           /* transparent */
    assign status[DATA_VALID]      = data_valid;     /* sticky      */
    assign status[7:2] = 6'd0;   /* unused bits */
    
    /* Instantiate the status register */ 
    cy_psoc3_status #(.cy_force_order(1), .cy_md_select(8'h02)) StsReg
    (
        /* input          */  .clock(op_clk),
        /* input  [07:00] */  .status(status)  
    );    
    
    /* The data input must be sampled one clock cycle before the end of the 
    *  ncs and nrd low pulses. This is the reason of use the udb_clock_enable
    *  to generate the clock to the status registers. 
    *  The clock is generated based on the input clock (used as clock_in for 
    *  the udb_clock_enable) and the condition that data are valid (used as 
    *  clock enable input for the udb_clock_enable component).
    */
    cy_psoc3_udb_clock_enable_v1_0 #(.sync_mode(`TRUE)) StsClkEn
    (
        /* input  */    .clock_in(clock),
        /* input  */    .enable(data_valid),
        /* output */    .clock_out(sts_clk)  /* Single pulse per read transaction */
    ); 
    
    /* Status register for lsb input data storage */   
    cy_psoc3_status #(.cy_force_order(1), .cy_md_select(8'hFF)) LsbReg
    (
        /* input          */  .clock(sts_clk),
        /* input  [07:00] */  .status(di_lsb)
    );
        
    /* GraphicLCDIntf State Machine States */
    localparam [3:0] GRAPH_LCD_INTF_STATE_IDLE            = 4'd0;
    localparam [3:0] GRAPH_LCD_INTF_STATE_LOAD_CMD        = 4'd1;
    localparam [3:0] GRAPH_LCD_INTF_STATE_LOAD_DATA       = 4'd2;
    localparam [3:0] GRAPH_LCD_INTF_STATE_WRITE1          = 4'd3;
    localparam [3:0] GRAPH_LCD_INTF_STATE_WRITE2          = 4'd11;    
    localparam [3:0] GRAPH_LCD_INTF_STATE_LOAD_LO_PULSE   = 4'd4;
    localparam [3:0] GRAPH_LCD_INTF_STATE_LOAD_HI_PULSE   = 4'd5;
    localparam [3:0] GRAPH_LCD_INTF_STATE_COUNT_LO        = 4'd6;
    localparam [3:0] GRAPH_LCD_INTF_STATE_COUNT_HI        = 4'd7;
    localparam [3:0] GRAPH_LCD_INTF_STATE_CMD             = 4'd8;
        
    /* The states mapped to state 8 and state 11 use the same datapath addresses
    *  as state 0 and state 3 appropriately. So the lower 3 bits of the state 
    *  machine can be used to directly drive the cs_addr bits. 
    */
    reg [3:0] state;
        
    always @(posedge op_clk)
    begin
        case(state)
        GRAPH_LCD_INTF_STATE_IDLE:
        begin
            state <= (cmd_ready)? GRAPH_LCD_INTF_STATE_LOAD_CMD : GRAPH_LCD_INTF_STATE_IDLE;
        end
        GRAPH_LCD_INTF_STATE_LOAD_CMD:
        begin
            state <= GRAPH_LCD_INTF_STATE_CMD;
        end
        GRAPH_LCD_INTF_STATE_CMD:
        begin
        /* Command decoding: if command bit is 1'b0 then COMMAND = WRITE, else COMMAND = READ */     
            if(cmd)
            begin
                state <= GRAPH_LCD_INTF_STATE_LOAD_LO_PULSE;
            end
            else
            begin
                if(data_ready)
                begin
                    state <= GRAPH_LCD_INTF_STATE_LOAD_DATA;
                end
                else
                begin
                    state <= GRAPH_LCD_INTF_STATE_CMD;
                end
            end
        end
        GRAPH_LCD_INTF_STATE_LOAD_DATA:
        begin
            state <= GRAPH_LCD_INTF_STATE_WRITE1;
        end
        GRAPH_LCD_INTF_STATE_WRITE1:
        begin
            state <= GRAPH_LCD_INTF_STATE_WRITE2;
        end
        GRAPH_LCD_INTF_STATE_WRITE2:
        begin
            state <= (cmd_ready)? GRAPH_LCD_INTF_STATE_LOAD_CMD : GRAPH_LCD_INTF_STATE_IDLE;
        end
        GRAPH_LCD_INTF_STATE_LOAD_LO_PULSE:
        begin
            state <= GRAPH_LCD_INTF_STATE_COUNT_LO;
        end
        GRAPH_LCD_INTF_STATE_COUNT_LO:
        begin
            state <= (z0_detect)? GRAPH_LCD_INTF_STATE_LOAD_HI_PULSE : GRAPH_LCD_INTF_STATE_COUNT_LO;
        end
        GRAPH_LCD_INTF_STATE_LOAD_HI_PULSE:
        begin
            state <= GRAPH_LCD_INTF_STATE_COUNT_HI;
        end
        GRAPH_LCD_INTF_STATE_COUNT_HI:
        begin
            state <= (z1_detect)? GRAPH_LCD_INTF_STATE_IDLE : GRAPH_LCD_INTF_STATE_COUNT_HI;
        end
        default: state <= GRAPH_LCD_INTF_STATE_IDLE;
        endcase
    end     /* end of state machine transition definition */
    
    /* Compute output signals. Change only on positive edge of clock */  
    always @(posedge op_clk)
    begin
        if(state == GRAPH_LCD_INTF_STATE_CMD)
        begin
            d_c <= data_lsb[0];
        end
    end
    
    always @(posedge op_clk)
    begin
        ncs <= ~(state == GRAPH_LCD_INTF_STATE_LOAD_DATA | state == GRAPH_LCD_INTF_STATE_WRITE1 |
                 state == GRAPH_LCD_INTF_STATE_LOAD_LO_PULSE | state == GRAPH_LCD_INTF_STATE_COUNT_LO);
    end
    
    always @(posedge op_clk)
    begin
        nrd <= ~(state == GRAPH_LCD_INTF_STATE_LOAD_LO_PULSE | state == GRAPH_LCD_INTF_STATE_COUNT_LO);
    end
    
    always @(posedge op_clk)
    begin
        nwr <= ~(state == GRAPH_LCD_INTF_STATE_LOAD_DATA);
    end
    
    always @(posedge op_clk)
    begin
        oe <= (state == GRAPH_LCD_INTF_STATE_LOAD_DATA | state == GRAPH_LCD_INTF_STATE_WRITE1);
    end
    
    assign do_lsb = data_lsb;
    assign data_valid = z0_detect & (state == GRAPH_LCD_INTF_STATE_COUNT_LO); 
        
    localparam lsb_config = {
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM0: Idle*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___F0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM1: LOAD_CMD */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM2: LOAD_DATA */
        `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM3:  Idle*/
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC___D0, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM4: LOAD_LO_PULSE */
        `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___D1,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM5: LOAD_HI_PULSE */
        `CS_ALU_OP__DEC, `CS_SRCA_A0, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC__ALU, `CS_A1_SRC_NONE,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM6: COUNT_LO */
        `CS_ALU_OP__DEC, `CS_SRCA_A1, `CS_SRCB_D0,
        `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC__ALU,
        `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
        `CS_CMP_SEL_CFGA, /*CFGRAM7: COUNT_HI */
        8'hFF, 8'h00,    /*CFG9: */
        8'hFF, 8'hFF,    /*CFG11-10: */
        `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
        `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
        `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
        `SC_SI_A_DEFSI, /*CFG13-12: */
        `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
        1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
        `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
        `SC_FB_NOCHN, `SC_CMP1_NOCHN,
        `SC_CMP0_NOCHN, /*CFG15-14:             */
        3'h00, `SC_FIFO_SYNC_NONE, 6'h00,    
        `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,   
        `SC_FIFO_LEVEL,`SC_FIFO_ASYNC,`SC_EXTCRC_DSBL,
        `SC_WRK16CAT_DSBL /*CFG17-16:              */
    };
       
    generate
        if(BitWidth == BIT_WIDTH_8)
        begin: GraphLcd8
            cy_psoc3_dp #(.d0_init(LoReadPulseCntPeriod), .d1_init(HiReadPulseCntPeriod),
            .cy_dpconfig(lsb_config)) Lsb
            (
                /* input */          .clk(op_clk),                      
                /* input [02:00] */  .cs_addr(state[2:0]), 
                /* input */          .route_si(1'b0),                  
                /* input */          .route_ci(1'b0),                  
                /* input */          .f0_load(1'b0),                  
                /* input */          .f1_load(1'b0),                  
                /* input */          .d0_load(1'b0),                  
                /* input */          .d1_load(1'b0),                  
                /* output */         .ce0(),                          
                /* output */         .cl0(),                          
                /* output */         .z0(z0_detect),                  
                /* output */         .ff0(),                          
                /* output */         .ce1(),                          
                /* output */         .cl1(),                          
                /* output */         .z1(z1_detect),                  
                /* output */         .ff1(),                          
                /* output */         .ov_msb(),                      
                /* output */         .co_msb(),                      
                /* output */         .cmsb(),                          
                /* output */         .so(),                          
                /* output */         .f0_bus_stat(cmd_not_full),      
                /* output */         .f0_blk_stat(cmd_empty),
                /* output */         .f1_bus_stat(data_not_full),          
                /* output */         .f1_blk_stat(data_empty),          
                /* input */          .ci(),                  
                /* output */         .co(),                  
                /* input */          .sir(),                  
                /* output */         .sor(),                  
                /* input */          .sil(),     
                /* output */         .sol(),     
                /* input */          .msbi(),     
                /* output */         .msbo(),     
                /* input [01:00] */  .cei(),     
                /* output [01:00] */ .ceo(),     
                /* input [01:00] */  .cli(),     
                /* output [01:00] */ .clo(),     
                /* input [01:00] */  .zi(),     
                /* output [01:00] */ .zo(),     
                /* input [01:00] */  .fi(),     
                /* output [01:00] */ .fo(),     
                /* input [01:00] */  .capi(),   
                /* output [01:00] */ .capo(),   
                /* input */          .cfbi(),     
                /* output */         .cfbo(),     
                /* input [07:00] */  .pi(),     
                /* output [07:00] */ .po(data_lsb)         
            );      
        end     /*  8-bit interface  */ 
        else
        begin: GraphLcd16
            wire [14:0] chain;       
            cy_psoc3_dp #(.d0_init(LoReadPulseCntPeriod), .d1_init(HiReadPulseCntPeriod),
            .cy_dpconfig(lsb_config)) Lsb
            (
                /* input */          .clk(op_clk),                     
                /* input [02:00] */  .cs_addr(state[2:0]),    
                /* input */          .route_si(1'b0),                 
                /* input */          .route_ci(1'b0),                 
                /* input */          .f0_load(1'b0),                  
                /* input */          .f1_load(1'b0),                  
                /* input */          .d0_load(1'b0),                  
                /* input */          .d1_load(1'b0),                  
                /* output */         .ce0(),                         
                /* output */         .cl0(),                         
                /* output */         .z0(z0_detect),                 
                /* output */         .ff0(),                         
                /* output */         .ce1(),                         
                /* output */         .cl1(),                         
                /* output */         .z1(z1_detect),                 
                /* output */         .ff1(),                         
                /* output */         .ov_msb(),                      
                /* output */         .co_msb(),                      
                /* output */         .cmsb(),                        
                /* output */         .so(),                          
                /* output */         .f0_bus_stat(cmd_not_full),    
                /* output */         .f0_blk_stat(cmd_empty),        
                /* output */         .f1_bus_stat(),                 
                /* output */         .f1_blk_stat(),                 
                /* input */          .ci(1'b0),                          
                /* output */         .co(chain[14]),                 
                /* input */          .sir(1'b0),                      
                /* output */         .sor(),                         
                /* input */          .sil(chain[12]),                 
                /* output */         .sol(chain[13]),                
                /* input */          .msbi(chain[11]),                
                /* output */         .msbo(),                        
                /* input [01:00] */  .cei(2'b0),              
                /* output [01:00] */ .ceo(chain[10:9]),      
                /* input [01:00] */  .cli(2'b0),              
                /* output [01:00] */ .clo(chain[8:7]),          
                /* input [01:00] */  .zi(2'b0),                  
                /* output [01:00] */ .zo(chain[6:5]),        
                /* input [01:00] */  .fi(2'b0),                  
                /* output [01:00] */ .fo(chain[4:3]),        
                /* input [01:00] */  .capi(2'b0),             
                /* output [01:00] */ .capo(chain[2:1]),      
                /* input */          .cfbi(1'b0),                     
                /* output */         .cfbo(chain[0]),                
                /* input [07:00] */  .pi(),                   
                /* output [07:00] */ .po(data_lsb)           
            );
             
            /* Status register for msb input data storage */            
            cy_psoc3_status #(.cy_force_order(1), .cy_md_select(8'hFF)) MsbReg
            (
                /* input          */  .clock(sts_clk),
                /* input  [07:00] */  .status(di_msb)
            );
    
            cy_psoc3_dp #(.cy_dpconfig(
            {
                `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM0: Idle*/
                `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM1: Idle*/
                `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC___F1,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM2: LOAD_DATA */
                `CS_ALU_OP_PASS, `CS_SRCA_A1, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM3: Idle*/
                `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM4: Idle*/
                `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM5: Idle*/
                `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM6: Idle*/
                `CS_ALU_OP_PASS, `CS_SRCA_A0, `CS_SRCB_D0,
                `CS_SHFT_OP_PASS, `CS_A0_SRC_NONE, `CS_A1_SRC_NONE,
                `CS_FEEDBACK_DSBL, `CS_CI_SEL_CFGA, `CS_SI_SEL_CFGA,
                `CS_CMP_SEL_CFGA, /*CFGRAM7: Idle*/
                8'hFF, 8'h00,    /*CFG9: */
                8'hFF, 8'hFF,    /*CFG11-10: */
                `SC_CMPB_A1_D1, `SC_CMPA_A1_D1, `SC_CI_B_ARITH,
                `SC_CI_A_ARITH, `SC_C1_MASK_DSBL, `SC_C0_MASK_DSBL,
                `SC_A_MASK_DSBL, `SC_DEF_SI_0, `SC_SI_B_DEFSI,
                `SC_SI_A_DEFSI, /*CFG13-12:              */
                `SC_A0_SRC_ACC, `SC_SHIFT_SL, 1'h0,
                1'h0, `SC_FIFO1_BUS, `SC_FIFO0_BUS,
                `SC_MSB_DSBL, `SC_MSB_BIT0, `SC_MSB_NOCHN,
                `SC_FB_NOCHN, `SC_CMP1_NOCHN,
                `SC_CMP0_NOCHN, /*CFG15-14:             */
                3'h00, `SC_FIFO_SYNC_NONE, 6'h00,    
                `SC_FIFO_CLK__DP,`SC_FIFO_CAP_AX,   
                `SC_FIFO_LEVEL,`SC_FIFO_ASYNC,`SC_EXTCRC_DSBL,
                `SC_WRK16CAT_DSBL /*CFG17-16:              */
            })) Msb(
                /* input */          .clk(op_clk),                     
                /* input [02:00] */  .cs_addr(state[2:0]),    
                /* input */          .route_si(1'b0),                 
                /* input */          .route_ci(1'b0),                 
                /* input */          .f0_load(1'b0),                  
                /* input */          .f1_load(1'b0),                  
                /* input */          .d0_load(1'b0),                  
                /* input */          .d1_load(1'b0),                  
                /* output */         .ce0(),                         
                /* output */         .cl0(),                         
                /* output */         .z0(),                          
                /* output */         .ff0(),                         
                /* output */         .ce1(),                         
                /* output */         .cl1(),                         
                /* output */         .z1(),                          
                /* output */         .ff1(),                         
                /* output */         .ov_msb(),                      
                /* output */         .co_msb(),                      
                /* output */         .cmsb(),                        
                /* output */         .so(),                          
                /* output */         .f0_bus_stat(),                 
                /* output */         .f0_blk_stat(),        
                /* output */         .f1_bus_stat(data_not_full),                 
                /* output */         .f1_blk_stat(data_empty),                 
                /* input */          .ci(chain[14]),                  
                /* output */         .co(),                          
                /* input */          .sir(chain[13]),                 
                /* output */         .sor(chain[12]),                
                /* input */          .sil(1'b0),                      
                /* output */         .sol(),                         
                /* input */          .msbi(1'b0),                     
                /* output */         .msbo(chain[11]),               
                /* input [01:00] */  .cei(chain[10:9]),          
                /* output [01:00] */ .ceo(),                 
                /* input [01:00] */  .cli(chain[8:7]),        
                /* output [01:00] */ .clo(),                 
                /* input [01:00] */  .zi(chain[6:5]),         
                /* output [01:00] */ .zo(),                  
                /* input [01:00] */  .fi(chain[4:3]),         
                /* output [01:00] */ .fo(),                  
                /* input [01:00] */  .capi(chain[2:1]),       
                /* output [01:00] */ .capo(),                
                /* input */          .cfbi(chain[0]),                 
                /* output */         .cfbo(),                        
                /* input [07:00] */  .pi(),                      
                /* output [07:00] */ .po(do_msb)             
            );
        end     /* 16-bit interface */
    endgenerate /* data bus bits definition */

endmodule

`endif /* GraphicLCDIntf_V_v1_00_ALREADY_INCLUDED */
