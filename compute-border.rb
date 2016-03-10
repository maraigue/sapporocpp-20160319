#!/usr/bin/env ruby
# -*- coding: utf-8 -*-

module Enumerable
  def extract_valid
    select{ |e| e }
  end
end

EPSILON = Math.sqrt(Float::MIN)

require 'csv'

data = CSV.read('Sample.csv')

labels = data.map{ |d| Integer(d.first) }
vectors = data.map{ |d| d[1..-1].map{ |e| Float(e) } }
n = labels.size

p labels
p vectors

coef = [1.0, 0.0]

while true
  max_update = 0.0
  
  for dim in 0..1
    breakpoints = n.times.map{ |i| vectors[i][dim] == 0 ? nil : {:id => i, :breakpoint => -(coef[1-dim]*vectors[i][1-dim] - 1.0)/vectors[i][dim] } }.extract_valid.sort_by{ |e| e[:breakpoint] }
    
    puts "---------- Dim #{dim} ----------"
    puts "<Breakpoints>"
    #breakpoints.each{ |b| puts "#{labels[b[:id]]}, #{vectors[b[:id]].inspect} | #{b.inspect}" }
    p breakpoints.map{ |b| b[:breakpoint] }
    
    grad_cumul = []
    grad_base = 0.0
    
    breakpoints.each do |b|
      grad = labels[b[:id]]*vectors[b[:id]][dim]
      
      if grad_cumul.empty?
        grad_cumul << grad.abs
      else
        grad_cumul << grad_cumul.last + grad.abs
      end
      grad_base += grad if grad < 0
    end
    p grad_cumul
    p grad_base
    
    best_breakpoint = grad_cumul.map{ |g| g + grad_base }.index{ |g| g >= 0 }
    if best_breakpoint
      update = (breakpoints[best_breakpoint][:breakpoint] - coef[dim]).abs
      coef[dim] = breakpoints[best_breakpoint][:breakpoint]
      #puts "coef[#{dim}] = breakpoints[#{best_breakpoint}][:breakpoint] = #{coef[dim]}"
      puts "coef[#{dim}] is updated. coef = #{coef.inspect}"
    else
      raise "No breakpoint found"
    end
    max_update = update if update > max_update
  end
  
  break if max_update < EPSILON
end
