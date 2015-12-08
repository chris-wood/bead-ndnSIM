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
data = subset(data, Type %in% c("InBeads", "OutBeads"))
data = subset(data, Node %in% c(160:189))

Nth.delete<-function(dataframe, n)dataframe[-(seq(n,to=nrow(dataframe),by=n)),]
data = Nth.delete(data, 1000)

data$Node = factor(data$Node)
data$Kilobits <- data$Kilobytes * 8
data$Type = factor(data$Type)

g.all <- ggplot(data, aes(x=Time, y=Kilobits, color=Type)) +
  geom_point(size=2) +
  geom_line() +
  ylab("Rate [Kbits/s]") +
  facet_wrap(~ Node) +
  theme_bw()

print(g.all)

png("test.png", width=500, height=250)
print(g.all)
retval <- dev.off()
