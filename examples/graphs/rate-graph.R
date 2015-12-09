# Copyright (c) 2012,2015  Alexander Afanasyev <alexander.afanasyev@ucla.edu>

# install.packages('ggplot2')
library(ggplot2)
# install.packages('scales')
library(scales)

# install.packages('doBy')
library(doBy)

#########################
# Rate trace processing #
#########################
data = read.table("dfn-rate-trace_0.5_1", header=T)
data = subset(data, Type %in% c("InData", "OutData"))
data = subset(data, Node %in% c(160:189))

Nth.delete<-function(dataframe, n)dataframe[-(seq(n,to=nrow(dataframe),by=n)),]
data = Nth.delete(data, 1000)

data$Node = factor(data$Node)
data$Kilobits <- data$Kilobytes * 8
data$Type = factor(data$Type)

g.all <- ggplot(data, aes(x=Time, y=Kilobits, color=Type), cex.lab=3, cey.lab=3, labels=c("InErase","OutErase")) +
  geom_point(size=2) +
  geom_line() +
  ylab("Rate [Kbits/s]") +
  xlab("Time [s]") + 
  #scale_fill_manual(values=c("#999999", "#E69F00"), breaks=c("InBeads","OutBeads"),labels=c("InErase","OutErase")) +
  #scale_colour_manual(name='', values=c('InErase'='blue', 'OutErase'='red')) +
  facet_wrap(~ Node) +
  theme(text = element_text(size=20))
  

print(g.all)

png("dfn-rate-trace_0.5_1.png", width=500, height=250)
print(g.all)
retval <- dev.off()
